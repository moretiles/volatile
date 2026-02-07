/*
 * nkht.h -- Hash table for null terminated keys, arbitrary values
 *
 * DERT - Miscellaneous Data Structures Library
 * https://github.com/moretiles/dert
 * Project licensed under Apache-2.0 license
 */

#pragma once

#include <ds/varray.h>
#include <ds/varena.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nkht {
    // keys placed here.
    void *keys;

    // size in bytes of each key.
    size_t key_size;

    // varray of varena
    Varray *key_backing_storage;

    // vals placed here.
    void *vals;

    // size in bytes of each val.
    size_t val_size;

    // number of keys that have associated values.
    size_t len;

    /*
     * current capacity.
     * used to check if we need to resize up.
     */
    size_t cap;

    //also need functions
} Nkht;

typedef struct nkht_iterator {
    size_t offset;
} Nkht_iterator;

// Allocates memory for and initializes a Nkht.
Nkht *nkht_create(size_t val_size);

// Initializes a Nkht.
int nkht_init(Nkht *table, size_t val_size);

// Deinitializes a Nkht.
void nkht_deinit(Nkht *table);

/*
 * Destroys a Nkht that was allocated by nkht_create.
 * Please, only use with memory allocated by nkht_create!
 */
void nkht_destroy(Nkht *table);

// Copies the contents of the value associated with key in table to dest.
int nkht_get(Nkht *table, const void *key, void *dest);

/*
 * Get the memory address of the value associated with key in table.
 * Should be used carefully as set and delete operations can freely overwrite contents at this memory location.
 * Thus, storing the pointer returned from this and then performing set and delete operations is a very bad idea!
 */
void *nkht_get_direct(Nkht *table, const void *key);

// Copies src to the value associated with key in table.
int nkht_set(Nkht *table, const void *key, const void *src);

// Prepare to iterate over elements of a table
int nkht_iterate_start(Nkht *table, Nkht_iterator *iterator);

// Get next element when iterating over elements of a table
int nkht_iterate_next(Nkht *table, Nkht_iterator *iterator, void *dest_key, void *dest_val);

// Get number of keys that have associated values in table
size_t nkht_len(Nkht *table);

#ifdef __cplusplus
}
#endif
