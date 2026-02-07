#include <ds/nkht.h>
#include <ds/nkht_priv.h>
#include <ds/pointerarith.h>
#include <siphash.h>

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/random.h>

// Initialize with dummy value of 0 that will be overwritten
uint8_t nkht_hash_salt[NKHT_HASH_SALT_LEN_EXPECTED] = { 0 };

// Set salt to random value
// If this fails we do not want to start
#ifndef NKHT_SIPHASH_RANDOMIZE_DISABLE
__attribute__((constructor))
void nkht_hash_salt_set_or_die(void) {
    assert(getrandom(nkht_hash_salt, NKHT_HASH_SALT_LEN_EXPECTED, 0) == NKHT_HASH_SALT_LEN_EXPECTED);
}
#endif

uint64_t nkht_hash_calc(const char *data, size_t len) {
    uint64_t hash;

    _Static_assert(NKHT_HASH_SALT_LEN_EXPECTED == 16, "Error: Macro defined constant NKHT_HASH_SALT_LEN_EXPECTED is not 16 bytes in length, unable to generate proper key for siphash.");

    siphash(data, len, &nkht_hash_salt, (uint8_t*) &hash, 64 / 8);

    return hash;
}

int nkht_hash_start(Nkht *table, const char *key, uint32_t *offset, uint32_t *iterate, struct nkht_key_bf **table_bf, void **table_key, void **table_val) {
    if(table == NULL || key == NULL || offset == NULL || iterate == NULL || table_bf == NULL || table_key == NULL || table_val == NULL) {
        return EINVAL;
    }

    // offset gets low bits, iterate gets high bits
    uint64_t hash = nkht_hash_calc(key, strlen(key));
    *offset = hash >> 0;
    *iterate = hash >> 32;

    // iterate must be odd otherwise may not reach all indices
    *iterate |= 0x1;

    *table_bf = nkht_hash_bf(table, *offset);
    *table_key = nkht_hash_key(table, *offset);
    *table_val = nkht_hash_val(table, *offset);
    if(table_bf == NULL || table_key == NULL || table_val == NULL) {
        return ENOTRECOVERABLE;
    }
    return 0;
}

int nkht_hash_next(Nkht *table, uint32_t *offset, uint32_t *iterate, struct nkht_key_bf **table_bf, void **table_key, void **table_val) {
    if(table == NULL || offset == NULL || iterate == NULL || table_bf == NULL || table_key == NULL || table_val == NULL) {
        return EINVAL;
    }

    *offset += *iterate;

    *table_bf = nkht_hash_bf(table, *offset);
    *table_key = nkht_hash_key(table, *offset);
    *table_val = nkht_hash_val(table, *offset);
    if(table_bf == NULL || table_key == NULL || table_val == NULL) {
        return ENOTRECOVERABLE;
    }

    return 0;
}

struct nkht_key_bf *nkht_hash_bf(Nkht *table, uint32_t offset) {
    struct nkht_key_bf *bf;
    if(table == NULL) {
        return NULL;
    }

    bf = pointer_literal_addition(table->keys, (offset % table->cap) * (sizeof(struct nkht_key_bf) + table->key_size));
    return bf;
}

void *nkht_hash_key(Nkht *table, uint32_t offset) {
    void *key;
    if(table == NULL) {
        return NULL;
    }

    key = nkht_hash_bf(table, offset);
    if(key == NULL) {
        return NULL;
    }
    key = pointer_literal_addition(key, sizeof(struct nkht_key_bf));
    return key;
}

void *nkht_hash_val(Nkht *table, uint32_t offset) {
    void *val;
    if(table == NULL) {
        return NULL;
    }

    val = pointer_literal_addition(table->vals, (offset % table->cap) * table->val_size);
    return val;
}

Nkht *nkht_create(size_t val_size) {
    return _nkht_create(NULL, val_size, NKHT_INITIAL_NUM_ELEMS);
}

Nkht *_nkht_create(Varray *key_backing_storage, size_t val_size, size_t num_elems) {
    Nkht *ret = calloc(1, sizeof(Nkht));
    if(ret == NULL) {
        return NULL;
    }

    if(_nkht_init(ret, key_backing_storage, val_size, num_elems) != 0) {
        free(ret);
        return NULL;
    }
    return ret;
}

int nkht_init(Nkht *table, size_t val_size) {
    return _nkht_init(table, NULL, val_size, NKHT_INITIAL_NUM_ELEMS);
}

int _nkht_init(Nkht *table, Varray *key_backing_storage, size_t val_size, size_t num_elems) {
    int ret = 0;
    if(table == NULL || val_size == 0) {
        return EINVAL;
    }

    if(num_elems == 0) {
        num_elems = NKHT_INITIAL_NUM_ELEMS;
    }

    table->keys = NULL;
    table->vals = NULL;
    table->key_backing_storage = NULL;
    Varena *first_arena = NULL;

    // Need bitfield to store additional information
    table->key_size = sizeof(void *);
    table->keys = calloc(num_elems, sizeof(struct nkht_key_bf) + table->key_size);
    if(table->keys == NULL) {
        ret = ENOMEM;
        goto _nkht_init_err;
    }

    table->val_size = val_size;
    table->vals = calloc(num_elems, val_size);
    if(table->vals == NULL) {
        ret = ENOMEM;
        goto _nkht_init_err;
    }

    if(key_backing_storage != NULL) {
        table->key_backing_storage = key_backing_storage;
    } else {
        table->key_backing_storage = varray_create(sizeof(Varena *));
        if(table->key_backing_storage == NULL) {
            ret = ENOMEM;
            goto _nkht_init_err;
        }

        ret = varray_realloc(table->key_backing_storage, 1);
        if(ret) {
            goto _nkht_init_err;
        }

        first_arena = varena_create(NKHT_INITIAL_KEY_BACKING_STORE_CAP);
        if(first_arena == NULL) {
            ret = ENOMEM;
            goto _nkht_init_err;
        }

        _Static_assert(
            NKHT_INITIAL_KEY_BACKING_STORE_CAP > (2 * sizeof(void *)),
            "NKHT_INITIAL_KEY_BACKING_STORE_CAP is too small!"
        );
        ret = varena_claim(&first_arena, NKHT_INITIAL_KEY_BACKING_STORE_CAP - (2 * sizeof(void *)));
        if(ret) {
            goto _nkht_init_err;
        }

        ret = varray_set(table->key_backing_storage, 0, &first_arena);
        if(ret) {
            goto _nkht_init_err;
        }
    }

    table->len = 0;
    table->cap = num_elems;

_nkht_init_err:
    if(ret && table->keys) {
        free(table->keys);
    }
    if(ret && table->vals) {
        free(table->vals);
    }
    if(ret && table->key_backing_storage) {
        varray_destroy(table->key_backing_storage);
    }
    if(ret && first_arena) {
        varena_destroy(&first_arena);
    }
    return ret;
}

void nkht_deinit(Nkht *table) {
    if(table == NULL) {
        return;
    }

    free(table->keys);
    free(table->vals);
    for(size_t i = 0; i < varray_len(table->key_backing_storage); i++) {
        Varena **current_arena = NULL;
        current_arena = varray_get_direct(table->key_backing_storage, i);
        if(current_arena == NULL) {
            // unrecoverable error
            continue;
        }
        varena_destroy(current_arena);
    }
    varray_destroy(table->key_backing_storage);
    return;
}

void nkht_destroy(Nkht *table) {
    if(table == NULL) {
        return;
    }

    nkht_deinit(table);
    free(table);
    return;
}

int nkht_get(Nkht *table, const void *key, void *dest) {
    void *src;
    if(table == NULL || key == NULL || dest == NULL) {
        return EINVAL;
    }

    src = nkht_get_direct(table, key);
    if(src == NULL) {
        return ENODATA;
    }

    if(memcpy(dest, src, table->val_size) != dest) {
        return ENOTRECOVERABLE;
    }
    return 0;
}

void *nkht_get_direct(Nkht *table, const void *key) {
    size_t remaining_guesses;
    uint32_t offset, iterate;
    struct nkht_key_bf *table_bf;
    void *table_key, *table_val;
    if(table == NULL || key == NULL) {
        return NULL;
    }

    remaining_guesses = table->cap;
    if(nkht_hash_start(table, key, &offset, &iterate, &table_bf, &table_key, &table_val) != 0) {
        return NULL;
    }

    while(remaining_guesses > 0 && table_bf != NULL && table_bf->occupied && table_key != NULL && table_val != NULL) {
        if(!nkht_strcmp((const char *) key, (const char **) table_key)) {
            return table_val;
        }
        remaining_guesses--;
        if(nkht_hash_next(table, &offset, &iterate, &table_bf, &table_key, &table_val) != 0) {
            return NULL;
        }
    }

    return NULL;
}

int nkht_set(Nkht *table, const void *key, const void *src) {
    return _nkht_set(table, key, src, true);
}

int _nkht_set(Nkht *table, const void *key, const void *src, bool need_to_copy) {
    size_t remaining_guesses;
    uint32_t offset, iterate;
    struct nkht_key_bf *table_bf;
    void *table_key, *table_val;
    if(table == NULL || key == NULL || src == NULL) {
        return EINVAL;
    }

    if((table->len * 4) >= table->cap) {
        nkht_double(table);
    }

    remaining_guesses = table->cap;
    if(nkht_hash_start(table, key, &offset, &iterate, &table_bf, &table_key, &table_val) != 0) {
        return ENOTRECOVERABLE;
    }

    while(remaining_guesses > 0 && table_bf != NULL && table_key != NULL && table_val != NULL) {
        if(!table_bf->occupied) {
            memset(table_bf, 0, sizeof(struct nkht_key_bf));
            table_bf->occupied = true;

            if(need_to_copy) {
                void *backing_storage_for_table_key = nkht_strdup(table, key);
                if(backing_storage_for_table_key == NULL) {
                    return ENOTRECOVERABLE;
                }
                memcpy(table_key, &backing_storage_for_table_key, table->key_size);
            } else {
                memcpy(table_key, &key, table->key_size);
            }
            memcpy(table_val, src, table->val_size);
            table->len++;
            return 0;
        } else if(table_bf->occupied && !nkht_strcmp((const char *) key, (const char **) table_key)) {
            memcpy(table_val, src, table->val_size);
            return 0;
        }

        remaining_guesses--;
        if(nkht_hash_next(table, &offset, &iterate, &table_bf, &table_key, &table_val) != 0) {
            return ENOTRECOVERABLE;
        }
    }

    return EXFULL;
}

int nkht_double(Nkht *table) {
    Nkht new_table;
    uint32_t offset, iterate;
    struct nkht_key_bf *table_bf;
    void *psuedorandom_sequence, *table_key, *table_val;
    size_t remaining_positions;
    if(table == NULL) {
        return EINVAL;
    }

    if(_nkht_init(&new_table, table->key_backing_storage, table->val_size, 4 * table->cap) != 0) {
        return ENOMEM;
    }
    psuedorandom_sequence = malloc(table->key_size);
    if(psuedorandom_sequence == NULL) {
        free(psuedorandom_sequence);
        return ENOMEM;
    }
    for(size_t i = 0; i < table->key_size; i += NKHT_HASH_SALT_LEN_EXPECTED) {
        memcpy(pointer_literal_addition(psuedorandom_sequence, i), nkht_hash_salt, NKHT_HASH_SALT_LEN_EXPECTED % (table->key_size - i));
    }
    ((char *) psuedorandom_sequence)[table->key_size - 1] = 0;

    remaining_positions = nkht_cap(table);
    if(nkht_hash_start(table, psuedorandom_sequence, &offset, &iterate, &table_bf, &table_key, &table_val) != 0) {
        free(psuedorandom_sequence);
        return ENOTRECOVERABLE;
    }
    free(psuedorandom_sequence);
    while(remaining_positions-- > 0) {
        if(nkht_hash_next(table, &offset, &iterate, &table_bf, &table_key, &table_val) != 0) {
            return ENOTRECOVERABLE;
        }

        if(table_bf->occupied) {
            if(_nkht_set(&new_table, *((char **) table_key), table_val, false) != 0) {
                return ENOTRECOVERABLE;
            }
        } else {
            continue;
        }
    }

    table->key_backing_storage = NULL;
    nkht_deinit(table);
    if(memcpy(table, &new_table, sizeof(Nkht)) != table) {
        return ENOTRECOVERABLE;
    }
    return 0;
}

void *nkht_strdup(Nkht *table, const void *key) {
    if(table == NULL || key == NULL) {
        return NULL;
    }

    const size_t key_len = strlen((char *) key);
    const size_t varray_current_len = varray_len(table->key_backing_storage);
    Varena **current_arena_ptr = varray_get_direct(
                                     table->key_backing_storage, varray_current_len - 1
                                 );
    if(current_arena_ptr == NULL) {
        return NULL;
    }

    size_t current_arena_cap = varena_arena_cap(*current_arena_ptr);
    void *duplicate = duplicate = varena_alloc(current_arena_ptr, key_len + 1);
    while(duplicate == NULL) {
        size_t varray_new_len = varray_len(table->key_backing_storage) + 1;
        int ret = varray_realloc(table->key_backing_storage, varray_new_len);
        if(ret) {
            return NULL;
        }
        current_arena_cap *= 2;
        Varena *new_arena = varena_create(current_arena_cap);
        if(new_arena == NULL) {
            return NULL;
        }
        _Static_assert(
            NKHT_INITIAL_KEY_BACKING_STORE_CAP > (2 * sizeof(void *)),
            "NKHT_INITIAL_KEY_BACKING_STORE_CAP is too small!"
        );
        ret = varena_claim(&new_arena, varena_arena_cap(new_arena) - (2 * sizeof(void *)));
        if(ret) {
            return NULL;
        }
        ret = varray_set(table->key_backing_storage, varray_new_len - 1, &new_arena);
        if(ret) {
            return NULL;
        }
        duplicate = varena_alloc(&new_arena, key_len + 1);
    }
    strcpy(duplicate, key);

    return duplicate;
}

int nkht_strcmp(const char *s1, const char **s2_ptr) {
    if(s1 == NULL || s2_ptr == NULL) {
        return 0;
    }

    const char *s2 = *s2_ptr;
    if(s2 == NULL) {
        return 0;
    }

    return strcmp(s1, s2);
}

int nkht_iterate_start(Nkht *table, Nkht_iterator *iterator) {
    if(table == NULL || iterator == NULL) {
        return EINVAL;
    }

    iterator->offset = 0;
    return 0;
}

int nkht_iterate_next(Nkht *table, Nkht_iterator *iterator, void *dest_key, void *dest_val) {
    struct nkht_key_bf *key_bf;
    void *key, *val;
    if(table == NULL || iterator == NULL || dest_key == NULL || dest_val == NULL) {
        return EINVAL;
    }

    while(iterator->offset < table->cap) {
        key_bf = nkht_hash_bf(table, iterator->offset);
        if(key_bf == NULL) {
            return ENOTRECOVERABLE;
        } else if (key_bf->occupied) {
            key = nkht_hash_key(table, iterator->offset);
            if(key == NULL) {
                return ENOTRECOVERABLE;
            }

            if(memcpy(dest_key, key, table->key_size) != dest_key) {
                return ENOTRECOVERABLE;
            }

            val = nkht_hash_val(table, iterator->offset);
            if(val == NULL) {
                return ENOTRECOVERABLE;
            }

            if(memcpy(dest_val, val, table->val_size) != dest_val) {
                return ENOTRECOVERABLE;
            }

            iterator->offset += 1;
            return 0;
        }

        iterator->offset += 1;
    }

    return ENODATA;
}

size_t nkht_len(Nkht *table) {
    if(table == NULL) {
        return 0;
    }

    return table->len;
}

size_t nkht_cap(Nkht *table) {
    if(table == NULL) {
        return 0;
    }

    return table->cap;
}
