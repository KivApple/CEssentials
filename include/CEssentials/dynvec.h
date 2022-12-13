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
 * @brief Generic dynamic vector (continuous storage container).
 * @details
 * Example of usage:
 * \code
 * dynvec(int) v;
 * dynvec_init(v);
 *
 * dynvec_push(v, int, 10);
 * dynvec_push(v, int, 20);
 * dynvec_for_each(v, i) {
 *     printf("%i\n", dynvec_at(v, i));
 * }
 *
 * dynvec_destroy(v);
 * \endcode
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/** Dynamic vector struct definition. */
#define dynvec(element_type) struct { element_type *data; size_t size, capacity; }

static inline bool dynvec_try_realloc(void **ptr, size_t new_size) {
	void *new_ptr = realloc(*ptr, new_size);
	if (new_ptr || !new_size) {
		*ptr = new_ptr;
		return true;
	} else {
		return false;
	}
}

/** Empty dynamic vector initialization.
 *
 * No heap allocation will be performed.
 */
#define dynvec_init(v) do { (v).data = NULL; (v).size = (v).capacity = 0; } while (0)

/** Destroy dynamic vector.
 *
 * If the vector holds complex elements you might need to destroy them first.
 */
#define dynvec_destroy(v) do { free((v).data); } while (0)

/**
 * Get dynamic vector size.
 */
#define dynvec_size(v) ((v).size)

/**
 * Get dynamic vector capacity.
 *
 * The vector can store up to capacity elements without reallocation.
 */
#define dynvec_capacity(v) ((v).capacity)

/**
 * Get dynamic vector element.
 */
#define dynvec_at(v, i) ((v).data[(i)])

/**
 * Clear dynamic vector.
 */
#define dynvec_clear(v) do { (v).size = 0; } while (0)

/**
 * Try to increase vector capacity if its less than desired.
 *
 * Returns `true` on success and `false` on memory allocation failure.
 *
 * The vector remains untouched if memory allocation fails.
 */
#define dynvec_reserve(v, new_capacity, element_type) ( \
	(new_capacity) > (v).capacity ? \
		( \
            (new_capacity) * sizeof(element_type) > (v).capacity * sizeof(element_type) ? ( \
				dynvec_try_realloc((void**) &((v).data), (new_capacity) * sizeof(element_type)) ? \
					((v).capacity = (new_capacity), true) : \
					false \
			) : false /* Integer overflow */ \
		) : \
		true \
)

/**
 * Resize vector.
 *
 * The vector remains untouched if memory allocation fails.
 *
 * If vector size increased, the contents of new elements are undefined.
 *
 * Returns `true` on success and `false` on memory allocation failure.
 *
 * The vector remains untouched if memory allocation fails.
 */
#define dynvec_resize(v, new_size, element_type) ( \
	dynvec_reserve((v), (new_size), element_type) ? \
		((v).size = (new_size), true) : \
		false \
)

/**
 * Appends an element to the end of the vector.
 *
 * Returns a pointer to the new element or NULL in case of memory failure.
 *
 * The vector remains untouched if memory allocation fails.
 */
#define dynvec_append(v, element_type) ( \
	(v).size < (v).capacity ? \
		&dynvec_at((v), (v).size++) : \
		( \
            (v).capacity <= SIZE_MAX / 2 ? ( \
				dynvec_reserve((v), (v).capacity ? (v).capacity * 2 : 16, element_type) ? \
					&dynvec_at((v), (v).size++) : \
					NULL \
			) : NULL /* Integer overflow */ \
		) \
)

/**
 * Appends an element to the end of the vector.
 *
 * Returns true on success and false on memory allocation failure.
 *
 * The vector remains untouched if memory allocation fails.
 */
#define dynvec_push(v, element_type, element) ( \
	(v).size < (v).capacity ? \
		((dynvec_at((v), (v).size++) = (element)), true) : ( \
            (v).capacity <= SIZE_MAX / 2 ? ( \
				dynvec_reserve((v), (v).capacity ? (v).capacity * 2 : 16, element_type) ? \
					(dynvec_at((v), (v).size++) = (element), true) : \
					false                                      \
			) : false /* Integer overflow */ \
		) \
)

/**
 * For each loop for the vector.
 */
#define dynvec_for_each(v, index) for (size_t index = 0; index < (v).size; index++)
