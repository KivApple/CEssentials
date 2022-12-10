/*
Copyright 2022 Ivan Kolesnikov <kiv.apple@gmail.com>
Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

/**
 * @file
 * @brief Generic hash table with quadratic probing.
 * @details
 * Example of usage:
 * \code
 * HT(const char*, int) ht; size_t index; int absent;
 * ht_init(ht);
 *
 * ht_put_str(ht, int, "10", index, absent);
 * ht_value(ht, index) = 10;
 *
 * ht_put_str(ht, int, "20", index, absent);
 * ht_value(ht, index) = 20;
 *
 * ht_for_each(ht, i) {
 *     printf("%s=%i\n", ht_key(ht, i), ht_value(ht, i));
 * }
 *
 * ht_get_str(ht, "20", index);
 * if (ht_valid(ht, index)) {
 *     printf("%i\n", ht_value(ht, i));
 *     ht_delete(ht, i);
 * } else {
 *     printf("Not found\n");
 * }
 *
 * ht_destroy(ht);
 * \endcode
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "roundup.h"

/** A hash table struct definition */
#define HT(key_type, value_type) struct { \
	size_t size, max_size, capacity; \
	char *flags; \
	key_type *keys; \
	value_type *values; \
}

/** Initialize a empty hash table (no memory allocation performed). */
#define ht_init(h) do { \
	(h).size = (h).max_size = (h).capacity = 0; \
	(h).flags = NULL; \
	(h).keys = NULL; \
	(h).values = NULL; \
} while (0)

/**
 * Destroy a hash table.
 *
 * You might need manually destroy keys and values if they are complex (e.g. nested heap allocated pointers).
 */
#define ht_destroy(h) do { free((h).values); free((h).keys); free((h).flags); } while (0)

/** Get number of elements stored in the hash table. */
#define ht_size(h) ((h).size)

/** Get maximum number of elements that can be stored in the hash table before rehashing. */
#define ht_max_size(h) ((h).max_size)

/** Get total hash table size. */
#define ht_capacity(h) ((h).capacity)

/** Clear hash table */
#define ht_clear(h) do { (h).size = 0; memset((h).flags, 0, (h).capacity); } while (0)

/**
 * Resize hash table to be able to hold at least new_capacity elements.
 * Success will be assigned to false in case of memory allocation failure.
 */
#define ht_reserve(h, key_type, value_type, new_capacity, success, hash_func) do { \
	if (new_capacity <= (h).max_size) { \
		(success) = true; \
		break; \
	} \
	size_t ht_new_capacity = (new_capacity) + ((new_capacity) >> 1) + ((new_capacity) >> 2); \
	roundupsize(ht_new_capacity); \
	if (ht_new_capacity < (new_capacity)) { /* Integer overflow */ \
		(success) = false; \
		break; \
	} \
	if (ht_new_capacity <= (h).capacity) { \
		ht_new_capacity <<= 1; \
	} \
	if (ht_new_capacity <= (h).capacity) { /* Integer overflow */ \
		(success) = false; \
		break; \
	} \
	char *ht_new_flags = malloc(ht_new_capacity); \
	if (!ht_new_flags) { \
		(success) = false; \
		break; \
	} \
	key_type *ht_new_keys = malloc(ht_new_capacity * sizeof(key_type)); \
	if (!ht_new_keys) { \
		free(ht_new_flags); \
		(success) = false; \
		break; \
	} \
	value_type *ht_new_values = malloc(ht_new_capacity * sizeof(value_type)); \
	if (!ht_new_values) { \
		free(ht_new_keys); \
		free(ht_new_values); \
		(success) = false; \
		break; \
	} \
	memset(ht_new_flags, 0, ht_new_capacity); \
	size_t ht_mask = ht_new_capacity - 1; \
	for (size_t ht_i = 0; ht_i < (h).capacity; ht_i++) { \
		if ((h).flags[ht_i] != 1) continue; \
		size_t ht_j = hash_func((h).keys[ht_i]) & ht_mask; \
		size_t ht_step = 0; \
		while (ht_new_flags[ht_j]) { \
			ht_j = (ht_j + ++ht_step) & ht_mask; \
		} \
		ht_new_flags[ht_j] = 1; \
		ht_new_keys[ht_j] = (h).keys[ht_i]; \
		ht_new_values[ht_j] = (h).values[ht_i]; \
	} \
	free((h).values); \
	free((h).keys); \
	free((h).flags); \
	(h).flags = ht_new_flags; \
	(h).keys = ht_new_keys; \
	(h).values = ht_new_values; \
	(h).capacity = ht_new_capacity; \
	(h).max_size = (ht_new_capacity >> 1) + (ht_new_capacity >> 2); \
	(success) = true; \
} while (0)

/**
 * Perform hash table lookup and return in \p result index of matched element if any.
 *
 * You have to check returned value with ht_valid() to determine if the element has been found.
 * Then you can use ht_key() and ht_value() to access it.
 */
#define ht_get(h, key, result, hash_func, eq_func) do { \
	if (!(h).size) { \
		(result) = 0; \
		break; \
	}                                                      \
	size_t ht_mask = (h).capacity - 1;                     \
	(result) = hash_func(key) & ht_mask;                   \
	size_t ht_step = 0;                                    \
	while ((h).flags[(result)] == 2 || ((h).flags[(result)] == 1 && !eq_func((h).keys[(result)], (key)))) { \
		(result) = ((result) + ++ht_step) & ht_mask; \
	} \
} while (0)

/**
 * Inserts an element inside the hash table and returns its index.
 *
 * \p absent specifies the operation result. 1 means that element was successfully inserted.
 * 0 means that element was already existed in the hash table (and its index was returned),
 * -1 means that memory allocation failure happens (\p index won't be assigned and shouldn't be used
 * ever with ht_valid()).
 */
#define ht_put(h, key_type, value_type, key, index, absent, hash_func, eq_func) do { \
	bool ht_success; \
	size_t ht_new_size = (h).size ? (h).size + 1 : 2; \
	if (ht_new_size < (h).size) { /* Integer overflow */ \
		(absent) = -1; \
		break; \
	}\
	ht_reserve((h), key_type, value_type, ht_new_size, ht_success, hash_func); \
	if (!ht_success) { \
		(absent) = -1; \
		break; \
	}                                                      \
	size_t ht_mask = (h).capacity - 1;                     \
	(index) = hash_func(key) & ht_mask;                   \
	size_t ht_step = 0;                                    \
	while ((h).flags[(index)] == 2 || ((h).flags[(index)] == 1 && !eq_func((h).keys[(index)], (key)))) { \
		(index) = ((index) + ++ht_step) & ht_mask; \
	} \
	if ((h).flags[(index)] == 1) { \
		(absent) = 0; \
	} else { \
		(h).flags[(index)] = 1; \
		(h).keys[(index)] = (key); \
		(h).size++; \
		(absent) = 1; \
	} \
} while (0)

/** Deletes element from the hash table by its index. */
#define ht_delete(h, index) do { \
	(h).flags[(index)] = 2; \
	(h).size--; \
} while (0)

/** Return first index for iteration over hash table. */
#define ht_begin(h) (0)

/** Return last index for iteration over hash table. */
#define ht_end(h) ((h).capacity)

/** Verify hash table element index for validity (needed for ht_get() and for iteration) */
#define ht_valid(h, index) ((h).flags && (h).flags[(index)] == 1)

/** Access key by hash table element index */
#define ht_key(h, index) ((h).keys[(index)])

/** Access value by hash table element index */
#define ht_value(h, index) ((h).values[(index)])

static inline size_t ht_next_valid_index(const char *flags, size_t capacity, size_t index) {
	while (index < capacity && flags[index] != 1) {
		index++;
	}
	return index;
}

/** For each loop over the hash table using provided \p index variable.
 *
 * You don't need to check index validity before access keys and values.
 * You can call kt_delete() on provided index. */
#define ht_for_each(h, index) for ( \
	size_t index = ht_next_valid_index((h).flags, (h).capacity, ht_begin((h))); \
	index != ht_end((h)) && ht_valid((h), index); \
	index++, index = ht_next_valid_index((h).flags, (h).capacity, index) \
)

/** Default hash implementation for integers */
#define ht_int_hash(x) ((size_t) (x))

/** Default equality implementation for integers */
#define ht_int_eq(a, b) ((a) == (b))

/** Reserve implementation for the hash table with integer keys */
#define ht_reserve_int(h, value_type, new_capacity, success) \
ht_reserve((h), int, value_type, (new_capacity), (success), ht_int_hash, ht_int_eq)

/** Lookup implementation for the hash table with integer keys */
#define ht_get_int(h, key, result) \
ht_get((h), (key), (result), ht_int_hash, ht_int_eq)

/** Insertion implementation for the hash table with integer keys */
#define ht_put_int(h, value_type, key, index, absent) \
ht_put((h), int, value_type, (key), (index), (absent), ht_int_hash, ht_int_eq)

/** Default hash implementation for strings */
static inline size_t ht_str_hash(const char *s) {
	size_t h = (size_t) *s;
	if (h) {
		for(++s; *s; ++s) {
			h = (h << 5) - h + (size_t) *s;
		}
	}
	return h;
}

/** Default equality implementation for strings */
#define ht_str_eq(a, b) (strcmp((a), (b)) == 0)

/** Reserve implementation for the hash table with string keys */
#define ht_reserve_str(h, value_type, new_capacity, success) \
ht_reserve((h), const char*, value_type, (new_capacity), (success), ht_str_hash, ht_str_eq)

/** Lookup implementation for the hash table with string keys */
#define ht_get_str(h, key, result) \
ht_get((h), (key), (result), ht_str_hash, ht_str_eq)

/** Insertion implementation for the hash table with string keys */
#define ht_put_str(h, value_type, key, index, absent) \
ht_put((h), const char*, value_type, (key), (index), (absent), ht_str_hash, ht_str_eq)

/** Combine two hash values to get a new one. Useful for writing composite key hash functions */
static inline size_t ht_hash_combine(size_t a, size_t b) {
	return a ^ (b + 0x9E3779B9 + (a << 6) + (a >> 2));
}
