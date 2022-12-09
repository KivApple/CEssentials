#pragma once
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

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#if SIZE_MAX == UINT64_MAX
#define CHashTable_roundUpCapacity(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, (x)|=(x)>>32, ++(x))
#elif SIZE_MAX == UINT32_MAX
#define CHashTable_roundUpCapacity(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))
#elif SIZE_MAX == UINT16_MAX
#define CHashTable_roundUpCapacity(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, ++(x))
#else
#error "Unsupported bitness"
#endif

#define CHashTable_TYPEDEF(name, key_type, value_type) \
	typedef struct name { \
		size_t size, capacity, maxSize; \
		char *flags; \
		key_type *keys; \
		value_type *values; \
	} name;

#define CHashTable_IMPL(name, key_type, value_type, has_values, hash_func, eq_func) \
	static inline void name##_init(name *table) { \
		table->size = 0; \
		table->capacity = 0; \
		table->maxSize = 0; \
		table->flags = NULL; \
		table->keys = NULL; \
		table->values = NULL; \
	} \
	static inline void name##_destroy(name *table) { \
		free(table->values); \
		free(table->keys); \
		free(table->flags); \
	} \
	static inline size_t name##_size(const name *table) { \
		return table->size; \
	}\
	static inline size_t name##_capacity(const name *table) { \
		return table->maxSize; \
	} \
	static inline size_t name##_totalCapacity(const name *table) { \
		return table->capacity; \
	} \
	static inline void name##_clear(name *table) { \
		if (table->size) { \
			table->size = 0; \
			memset(table->flags, 0, table->capacity); \
		} \
	} \
	static inline bool name##_reserve(name *table, size_t newCapacity) { \
		if (newCapacity <= table->maxSize) return true; \
		newCapacity += newCapacity / 2 + newCapacity / 4; \
		CHashTable_roundUpCapacity(newCapacity); \
		if (newCapacity <= table->capacity) newCapacity *= 2; \
		char *newFlags = malloc(newCapacity); \
		if (!newFlags) return false; \
		key_type *newKeys = malloc(newCapacity * sizeof(key_type));     \
		if (!newKeys) { \
			free(newFlags); \
			return false; \
		} \
		value_type *newValues = has_values ? malloc(newCapacity * sizeof(value_type)) : NULL; \
		if (has_values && !newValues) { \
			free(newKeys); \
			free(newFlags); \
			return false; \
		} \
		memset(newFlags, 0, newCapacity); \
		size_t mask = newCapacity - 1; \
		for (size_t i = 0; i < table->capacity; i++) { \
			if (table->flags[i] != 1) continue; \
			size_t j = hash_func(table->keys[i]) & mask; \
			size_t step = 0; \
			while (newFlags[j]) { \
				j = (j + ++step) & mask; \
			} \
			newFlags[j] = 1; \
			newKeys[j] = table->keys[i]; \
			if (has_values) newValues[j] = table->values[i]; \
		} \
		free(table->flags); \
		free(table->keys); \
		free(table->values); \
		table->flags = newFlags; \
		table->keys = newKeys; \
		table->values = newValues; \
		table->capacity = newCapacity; \
		table->maxSize = newCapacity / 2 + newCapacity / 4; \
		return true; \
	} \
	static inline size_t name##_begin(const name *table) { \
		return 0; \
	} \
	static inline size_t name##_end(const name *table) { \
		return table->capacity; \
	} \
	static inline bool name##_valid(const name *table, size_t i) { \
		return table->flags && table->flags[i] == 1; \
	} \
	static inline key_type name##_key(const name *table, size_t i) { \
		return table->keys[i]; \
	} \
	static inline value_type *name##_value(name *table, size_t i) { \
		return table->values + i; \
	} \
	static inline size_t name##_get(const name *table, key_type key) { \
		if (table->size == 0) return 0; \
		size_t mask = table->capacity - 1; \
		size_t i = hash_func(key) & mask; \
		size_t step = 0; \
		while (table->flags[i] == 2 || (table->flags[i] == 1 && !eq_func(table->keys[i], key))) { \
			i = (i + ++step) & mask; \
		} \
		return i; \
	} \
	static inline size_t name##_put(name *table, key_type key, int *result) { \
		if (!name##_reserve(table, table->size ? table->size + 1 : 4)) { \
			*result = -1; \
			return SIZE_MAX; \
		} \
		size_t mask = table->capacity - 1; \
		size_t i; \
		i = hash_func(key) & mask; \
		size_t step = 0; \
		while (table->flags[i] == 1 && !eq_func(table->keys[i], key)) { \
			i = (i + ++step) & mask; \
		} \
		if (table->flags[i] == 1) { \
			*result = 0; \
		} else { \
			table->flags[i] = 1; \
			table->keys[i] = key; \
			table->size++; \
			*result = 1; \
		} \
		return i; \
	} \
	static inline void name##_delete(name *table, size_t i) { \
		table->flags[i] = 2; \
		table->size--; \
	}

static inline size_t CHashTable_intHash(int x) {
	return (size_t) x;
}

static inline bool CHashTable_intEq(int a, int b) {
	return a == b;
}

static inline size_t CHashTable_strHash(const char *s) {
	size_t h = (size_t) *s;
	if (h) {
		for(++s; *s; ++s) h = (h << 5) - h + (size_t) *s;
	}
	return h;
}

static inline bool CHashTable_strEq(const char *a, const char *b) {
	return strcmp(a, b) == 0;
}
