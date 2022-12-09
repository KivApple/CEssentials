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
#include <string.h>

#define CVec_TYPEDEF(name, element_type) \
	typedef struct name { \
    	size_t size; \
		size_t capacity; \
		element_type *data; \
	} name;

#define CVec_IMPL(name, element_type) \
	static inline void name##_init(name *v) { \
    	v->size = 0; \
		v->capacity = 0; \
		v->data = NULL; \
	} \
	static inline void name##_destroy(name *v) { \
    	free(v->data); \
	} \
	static inline size_t name##_size(const name *v) {  \
    	return v->size; \
	} \
	static inline size_t name##_capacity(const name *v) { \
    	return v->capacity; \
	} \
    static inline const element_type *name##_constAt(const name *v, size_t i) { \
    	return v->data + i; \
	} \
	static inline element_type *name##_at(name *v, size_t i) { \
    	return v->data + i; \
	} \
	static inline void name##_clear(name *v) { \
		v->size = 0; \
	} \
	static inline bool name##_reserve(name *v, size_t newCapacity) { \
		if (v->capacity >= newCapacity) return true; \
		element_type *newData = realloc(v->data, newCapacity * sizeof(element_type)); \
		if (!newData) return false; \
		v->data = newData; \
		v->capacity = newCapacity; \
		return true; \
	} \
	static inline bool name##_resize(name *v, size_t newSize) { \
		if (name##_reserve(v, newSize)) { \
            v->size = newSize; \
            return true; \
        } \
		return false; \
	} \
	static inline element_type *name##_append(name *v) { \
    	if (v->size == v->capacity) { \
            size_t newCapacity = v->capacity ? v->capacity * 2 : 16; \
            if (!name##_reserve(v, newCapacity)) { \
                return NULL; \
            } \
        } \
		element_type *element = v->data + v->size;  \
		v->size++; \
		return element; \
	} \
	static inline element_type *name##_push(name *v, const element_type *value) {\
        element_type *element = name##_append(v); \
		if (element) {                      \
        	memcpy(element, value, sizeof(element_type)); \
		} \
		return element; \
	}
