/*
 * nkht_priv.h -- Hash table for null terminated keys, arbitrary values
 *
 * DERT - Miscellaneous Data Structures Library
 * https://github.com/moretiles/dert
 * Project licensed under Apache-2.0 license
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Expected length as key for siphash
#define NKHT_HASH_SALT_LEN_EXPECTED (16)
// Used as default size for Nkht.
#define NKHT_INITIAL_NUM_ELEMS (16)
// Used as default backing storage size for the keys owned by the Nkht.
#define NKHT_INITIAL_KEY_BACKING_STORE_CAP (256)

/*
 * Global variable present in nkht.c
 * Used as the "key" with siphash
 */
extern uint8_t nkht_hash_salt[NKHT_HASH_SALT_LEN_EXPECTED];

// Special secret bitfield placed before all keys stored in Nkht
struct nkht_key_bf {
    // Is the associated key set?
    bool occupied : 1;
};

#ifndef NKHT_SIPHASH_RANDOMIZE_DISABLE
__attribute__((constructor)) void nkht_hash_salt_set_or_die(void);
#endif

/* Internal FNV hashing calculation
 * Top 32 bits used to calculate iterate.
 * Bottom 32 bits used to calculate offset.
 */
uint64_t nkht_hash_calc(const char *data, size_t len);

/*
 * Using the first len bytes of key calculate offset and iterate.
 * Then, find the nkht_key_bf, key, and value at offset in table.
 */
int nkht_hash_start(Nkht *table, const char *key, uint32_t *offset, uint32_t *iterate, struct nkht_key_bf **table_bf, void **table_key, void **table_val);

/*
 * Apply an additional step of the FNV hashing calculation updating offset and iterate.
 * Get the nkht_key_bf, key, and value at the offset in table.
 */
int nkht_hash_next(Nkht *table, uint32_t *offset, uint32_t *iterate, struct nkht_key_bf **table_bf, void **table_key, void **table_val);

// Get nkht_key_bf at offset in table.
struct nkht_key_bf *nkht_hash_bf(Nkht *table, uint32_t offset);

// Get key at offset in table.
void *nkht_hash_key(Nkht *table, uint32_t offset);

// Get val at offset in table.
void *nkht_hash_val(Nkht *table, uint32_t offset);

// Duplicate null-terminated key by placing in one of the backing_storage arenas of table.
void *nkht_strdup(Nkht *table, const void *key);

// Compare null terminated string in s1 with dereferenced null terminated string that is pointer to string in s2
int nkht_strcmp(const char *s1, const char **s2_ptr);

// Grow size of Nkht (by some amount).
int nkht_double(Nkht *table_ptr);

// Nkht_create with parameterized starting number of elements.
Nkht *_nkht_create(Varray *key_backing_storage, size_t val_size, size_t num_elems);

// Nkht_init with ability to set backing_storage and num_elems
int _nkht_init(Nkht *table, Varray *key_backing_storage, size_t val_size, size_t num_elems);

int _nkht_set(Nkht *table, const void *key, const void *src, bool need_to_copy);

// Current cap of table
size_t nkht_cap(Nkht *table);

#ifdef __cplusplus
}
#endif
