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
 * @brief Dynamic strings that are compatible with normal NULL-terminated C-strings - splitting and joining utilities.
 */

#include "dynstr.h"
#include "dynvec.h"

/**
 * Dynamic vector container to store string split result.
 */
typedef dynvec(dynstr) dynstr_list_t;

/**
 * Split \p s by separator \p separator and store the result in \p result
 * (must be initialized using dynvec_init() first).
 *
 * \p if max_split greater than zero, the result count cannot be greater than this limit.
 *
 * Returns false on memory allocation failure.
 */
bool str_split(const char *s, const char *separator, int max_split, dynstr_list_t *result);

/**
 * Joins back split result using \p separator.
 *
 * If \p dest is not NULL, contents will be appended to this string.
 */
dynstr dynstr_list_join(dynstr dest, dynstr_list_t *list, const char *separator);

/**
 * Frees all strings stored inside the \p list and clears it afterwards.
 *
 * You must call this function before dynvec_destroy() to avoid memory leaks.
 */
void dynstr_list_clear(dynstr_list_t *list);
