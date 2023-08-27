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
 * @brief Generic QuickSort algorithm implementation.
 * @details
 * Example of usage:
 * \code
 * int arr[] = {5, 3, 2, -10};
 * qsort_int(arr, 4);
 * // Now arr[] = {-10, 2, 3, 5}
 * \endcode
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <malloc.h>
#ifndef alloca
#define alloca _alloca
#endif
#else
#include <alloca.h>
#endif

#ifndef QSORT_MAX_STACK
#define QSORT_MAX_STACK 1024
#endif

#define qsort_swap(a, b, type) \
	do { type tmp = (a); (a) = (b); (b) = tmp; } while (0)

#define qsort_iterative(arr, l, h, type, cmp) \
	do { \
		size_t qsort_stackSize = (h) - (l) + 1; \
		size_t *qsort_stack = qsort_stackSize > QSORT_MAX_STACK ? malloc(qsort_stackSize * sizeof(size_t)) : alloca(qsort_stackSize * sizeof(size_t)); \
		ptrdiff_t qsort_top = -1; \
		qsort_stack[++qsort_top] = (l); \
		qsort_stack[++qsort_top] = (h); \
		while (qsort_top >= 0) { \
        	size_t qsort_h = qsort_stack[qsort_top--]; \
			size_t qsort_l = qsort_stack[qsort_top--]; \
			type qsort_x = (arr)[qsort_h]; \
			ptrdiff_t qsort_i = ((ptrdiff_t) qsort_l - 1); \
			for (size_t qsort_j = (qsort_l); qsort_j <= (qsort_h) - 1; qsort_j++) { \
				if (cmp(&(arr)[qsort_j], &qsort_x) < 0) { \
					qsort_i++; \
					qsort_swap((arr)[qsort_i], (arr)[qsort_j], type); \
				} \
			} \
			qsort_swap((arr)[qsort_i + 1], (arr)[(qsort_h)], type); \
			ptrdiff_t qsort_p = qsort_i + 1; \
			if (qsort_p - 1 > (ptrdiff_t) qsort_l) { \
            	qsort_stack[++qsort_top] = qsort_l; \
				qsort_stack[++qsort_top] = qsort_p - 1; \
            } \
			if (qsort_p + 1 < (ptrdiff_t) qsort_h) { \
            	qsort_stack[++qsort_top] = qsort_p + 1; \
				qsort_stack[++qsort_top] = qsort_h; \
            } \
		} \
		if (qsort_stackSize > QSORT_MAX_STACK) { \
			free(qsort_stack); \
		} \
	} while (0)

/** Sort provided array of given type using specified comparator (acts like strcmp/memcmp) */
#define qsort(arr, count, type, cmp) do { \
        if ((count) > 1) { \
            qsort_iterative((arr), 0, (count) - 1, type, cmp); \
        } \
    } while (0)

/** Comparator for numeric values (works for char, short, int, long, long long, float, double, long double) */
#define qsort_int_cmp(a, b) (*(a) > *(b) ? 1 : (*(a) < *(b) ? -1 : 0))

/** Sort int array */
#define qsort_int(arr, count) qsort((arr), (count), int, qsort_int_cmp)

/** Comparator for C-strings (uses strcmp) */
#define qsort_str_cmp(a, b) strcmp(*(a), *(b))

/** Sort array of C-strings (char*) */
#define qsort_str(arr, count) qsort((arr), (count), char*, qsort_str_cmp)

/** Sort array of immutable C-strings (const char*) */
#define qsort_str_const(arr, count) qsort((arr), (count), const char*, qsort_str_cmp)
