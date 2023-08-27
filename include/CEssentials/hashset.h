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
 * @brief Generic hash set with quadratic probing.
 * @details
 * Example of usage:
 * \code
 * HS(const char*) hs; size_t index; int absent;
 * hs_init(hs);
 *
 * hs_put_str(hs, "10", index, absent);
 *
 * hs_put_str(hs, "20", index, absent);
 *
 * hs_for_each(hs, i) {
 *     printf("%s\n", hs_key(hs, i));
 * }
 *
 * hs_get_str(hs, "20", index);
 * if (hs_valid(hs, index)) {
 *     printf("Found\n", i);
 *     hs_delete(hs, i);
 * } else {
 *     printf("Not found\n");
 * }
 *
 * hs_destroy(hs);
 * \endcode
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "roundup.h"

/** A hash set struct definition */
#define HS(key_type) struct { \
	size_t size, used, max_used, capacity; \
	char *flags; \
	key_type *keys; \
}

/** Initialize a empty hash set (no memory allocation performed). */
#define hs_init(h) do { \
	(h).size = (h).used = (h).max_used = (h).capacity = 0; \
	(h).flags = NULL; \
	(h).keys = NULL; \
} while (0)

/**
 * Destroy a hash set.
 *
 * You might need manually destroy keys if they are complex (e.g. nested heap allocated pointers).
 */
#define hs_destroy(h) do { free((h).keys); free((h).flags); } while (0)

/** Get number of elements stored in the hash set. */
#define hs_size(h) ((h).size)

/** Get maximum number of elements that are occupied in the hash set (inserted + deleted). */
#define hs_used(h) ((h).used)

/** Get maximum number of elements that can be stored in the hash set before rehashing. */
#define hs_max_used(h) ((h).max_used)

/** Get total hash set size. */
#define hs_capacity(h) ((h).capacity)

/** Clear hash set */
#define hs_clear(h) do { (h).size = 0; (h).used = 0; if ((h).flags) { memset((h).flags, 0, (h).capacity); } } while (0)

/**
 * Resize hash set to be able to hold at least new_capacity elements.
 * Success will be assigned to false in case of memory allocation failure.
 */
#define hs_reserve(h, key_type, new_capacity, success, hash_func) do { \
	if (new_capacity <= (h).max_used) { \
		(success) = true; \
		break; \
	} \
	size_t hs_new_capacity = (new_capacity); \
	roundupsize(hs_new_capacity); \
	if (hs_new_capacity < (new_capacity)) { /* Integer overflow */ \
		(success) = false; \
		break; \
	} \
	size_t hs_new_max_used = (hs_new_capacity >> 1) + (hs_new_capacity >> 2); \
	if (hs_new_max_used < (new_capacity)) { \
    	hs_new_capacity <<= 1; \
		if (hs_new_capacity < (new_capacity)) { /* Integer overflow */ \
			(success) = false; \
			break; \
		} \
		hs_new_max_used = (hs_new_capacity >> 1) + (hs_new_capacity >> 2); \
    } \
	char *hs_new_flags = malloc(hs_new_capacity); \
	if (!hs_new_flags) { \
		(success) = false; \
		break; \
	} \
	key_type *hs_new_keys = malloc(hs_new_capacity * sizeof(key_type)); \
	if (!hs_new_keys) { \
		free(hs_new_flags); \
		(success) = false; \
		break; \
	} \
	memset(hs_new_flags, 0, hs_new_capacity); \
	size_t hs_mask = hs_new_capacity - 1; \
	for (size_t hs_i = 0; hs_i < (h).capacity; hs_i++) { \
		if ((h).flags[hs_i] != 1) continue; \
		size_t hs_j = hash_func((h).keys[hs_i]) & hs_mask; \
		size_t hs_step = 0; \
		while (hs_new_flags[hs_j]) { \
			hs_j = (hs_j + ++hs_step) & hs_mask; \
		} \
		hs_new_flags[hs_j] = 1; \
		hs_new_keys[hs_j] = (h).keys[hs_i]; \
	} \
	free((h).keys); \
	free((h).flags); \
	(h).flags = hs_new_flags; \
	(h).keys = hs_new_keys; \
	(h).capacity = hs_new_capacity; \
	(h).used = (h).size; \
	(h).max_used = hs_new_max_used; \
	(success) = true; \
} while (0)

/**
 * Perform hash set lookup and return in \p result index of matched element if any.
 *
 * You have to check returned value with hs_valid() to determine if the element has been found.
 * Then you can use hs_key() to access it.
 */
#define hs_get(h, key, result, hash_func, eq_func) do { \
	if (!(h).size) { \
		(result) = 0; \
		break; \
	} \
	size_t hs_mask = (h).capacity - 1; \
	(result) = hash_func(key) & hs_mask; \
	size_t hs_step = 0; \
	while ((h).flags[(result)] == 2 || ((h).flags[(result)] == 1 && !eq_func((h).keys[(result)], (key)))) { \
		(result) = ((result) + ++hs_step) & hs_mask; \
	} \
} while (0)

/**
 * Insert an element inside the hash set and return its index.
 *
 * \p absent specifies the operation result. 1 means that the element was successfully inserted.
 * 0 means that the element with given key was already existed in the hash set (and its index was returned),
 * -1 means that memory allocation failure happened (\p index won't be assigned and shouldn't be used
 * even with hs_valid()).
 */
#define hs_put(h, key_type, key, index, absent, hash_func, eq_func) do { \
	bool hs_success; \
	size_t hs_new_size = (h).used ? (h).used + 1 : 2; \
	if (hs_new_size < (h).used) { /* Integer overflow */ \
		(absent) = -1; \
		break; \
	} \
	hs_reserve((h), key_type, hs_new_size, hs_success, hash_func); \
	if (!hs_success) { \
		(absent) = -1; \
		break; \
	} \
	size_t hs_mask = (h).capacity - 1; \
	(index) = hash_func(key) & hs_mask; \
	size_t hs_step = 0; \
	while ((h).flags[(index)] == 1 && !eq_func((h).keys[(index)], (key))) { \
		(index) = ((index) + ++hs_step) & hs_mask; \
	} \
	if ((h).flags[(index)] == 1) { \
		(absent) = 0; \
	} else { \
        if ((h).flags[(index)] == 0) { \
        	(h).used++; \
		} \
		(h).flags[(index)] = 1; \
		(h).keys[(index)] = (key); \
		(h).size++; \
		(absent) = 1; \
	} \
} while (0)

/** Delete an element from the hash set by its index. */
#define hs_delete(h, index) do { \
	(h).flags[(index)] = 2; \
	(h).size--; \
} while (0)

/** Return first index for iteration over hash set. */
#define hs_begin(h) (0)

/** Return last index for iteration over hash set. */
#define hs_end(h) ((h).capacity)

/** Verify hash set element index for validity (needed for hs_get() and for iteration) */
#define hs_valid(h, index) ((h).flags && (h).flags[(index)] == 1)

/** Access key by hash set element index */
#define hs_key(h, index) ((h).keys[(index)])

static inline size_t hs_next_valid_index(const char *flags, size_t capacity, size_t index) {
	while (index < capacity && flags[index] != 1) {
		index++;
	}
	return index;
}

/** For each loop over the hash set using provided \p index variable.
 *
 * You don't need to check index validity before access keys when using this macro.
 * You can safely call kt_delete() on provided index and either continue or break iteration. */
#define hs_for_each(h, index) for ( \
	size_t index = hs_next_valid_index((h).flags, (h).capacity, hs_begin((h))); \
	index != hs_end((h)) && hs_valid((h), index); \
	index++, index = hs_next_valid_index((h).flags, (h).capacity, index) \
)

/** Default hash implementation for integers */
#define hs_int_hash(x) ((size_t) (x))

/** Default equality implementation for integers */
#define hs_int_eq(a, b) ((a) == (b))

/** Reserve implementation for the hash set with integer keys */
#define hs_reserve_int(h, new_capacity, success) \
hs_reserve((h), int, (new_capacity), (success), hs_int_hash, hs_int_eq)

/** Lookup implementation for the hash set with integer keys */
#define hs_get_int(h, key, result) \
hs_get((h), (key), (result), hs_int_hash, hs_int_eq)

/** Insertion implementation for the hash set with integer keys */
#define hs_put_int(h, key, index, absent) \
hs_put((h), int, (key), (index), (absent), hs_int_hash, hs_int_eq)

/** Default hash implementation for strings */
static inline size_t hs_str_hash(const char *s) {
	size_t h = (size_t) *s;
	if (h) {
		for(++s; *s; ++s) {
			h = (h << 5) - h + (size_t) *s;
		}
	}
	return h;
}

/** Default equality implementation for strings */
#define hs_str_eq(a, b) (strcmp((a), (b)) == 0)

/** Reserve implementation for the hash set with string keys */
#define hs_reserve_str(h, new_capacity, success) \
hs_reserve((h), const char*, (new_capacity), (success), hs_str_hash, hs_str_eq)

/** Lookup implementation for the hash set with string keys */
#define hs_get_str(h, key, result) \
hs_get((h), (key), (result), hs_str_hash, hs_str_eq)

/** Insertion implementation for the hash set with string keys */
#define hs_put_str(h, key, index, absent) \
hs_put((h), const char*, (key), (index), (absent), hs_str_hash, hs_str_eq)

/** Combine two hash values to get a new one. Useful for writing composite key hash functions */
static inline size_t hs_hash_combine(size_t a, size_t b) {
	return a ^ (b + 0x9E3779B9 + (a << 6) + (a >> 2));
}
