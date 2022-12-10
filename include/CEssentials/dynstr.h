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
 * @brief Dynamic strings that are compatible with normal NULL-terminated C-string
 * @details
 * Example of usage:
 * \code
 * dynstr s = dynstr_new("Hello");
 * s = dynstr_push(s, ' ');
 * s = dynstr_append(s, "world");
 * printf("%s\n", s);
 * dynstr_free(s);
 * \endcode
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef _MSC_VER
#define DYNSTR_FMT_FUNC(fmt_index, first_arg_index) __attribute__((format(printf, fmt_index, first_arg_index)))
#define DYNSTR_FMT_ARG
#else
#define DYNSTR_FMT_FUNC(fmt_index, first_arg_index)
#define DYNSTR_FMT_ARG _Printf_format_string_
#endif

/** A typedef for dynamic strings (just an alias for `char*`). */
typedef char *dynstr;

/** A header that prepended to all dynstr. Normally shouldn't be accessed directly from user code. */
typedef struct dynstr_header {
	size_t size; //!< String length.
	size_t capacity; //!< String memory block length.
} dynstr_header_t;

/** Obtain a pointer to a dynamic string header. Normally shouldn't be accessed directly from user code. */
#define dynstr_header(s) (((dynstr_header_t*) (s)) - 1)

/** Get dynamic string length. Constant time operation. */
#define dynstr_size(s) (dynstr_header(s)->size)

/** Get dynamic string memory block length (how many characters can string hold without reallocation). */
#define dynstr_capacity(s) (dynstr_header(s)->capacity)

/** Allocate a empty dynamic string with specified capacity */
dynstr dynstr_alloc(size_t capacity);

/** Allocate a dynamic string from provided character array.
 *
 * If \p data is `NULL`, allocated a string of \p count characters without initialization. */
dynstr dynstr_new_chars(const char *data, size_t count);

/** Allocate a dynamic string copying contents from NULL-terminated string */
dynstr dynstr_new(const char *data);

/** Allocate a dynamic string using printf-formatted contents */
dynstr dynstr_new_printf(DYNSTR_FMT_ARG const char *fmt, ...) DYNSTR_FMT_FUNC(1, 2);

/** Allocate a dynamic string using printf-formatted contents using `va_list` arguments. */
dynstr dynstr_new_vprintf(const char *fmt, va_list args);

/** Allocate a dynamic string copying existing dynamic string */
dynstr dynstr_dup(dynstr s);

/** Free dynamic string. All dynamic strings should be freed by this function. Accepts NULL (as usual `free`). */
void dynstr_free(dynstr s);

/** Shrink dynamic string memory block to its actual length. */
dynstr dynstr_shrink(dynstr s);

/**
 * Increase dynamic string memory block to be able to hold at least \p capacity characters.
 *
 * If current string capacity is greater or equal to requested, does nothing.
 */
dynstr dynstr_reserve(dynstr s, size_t capacity);

/**
 * Increase dynamic string memory block to be able to hold at least \p capacity characters.
 *
 * If current string capacity is greater or equal than requested, does nothing.
 *
 * If current string capacity is less than desired capacity rounds up desired capacity to a nearest power of two
 * before reallocation.
 */
dynstr dynstr_reserve2(dynstr s, size_t capacity);

/**
 * Set dynamic string size.
 *
 * If \p size is greater than current string length, the contents of appended characters are undefined.
 */
dynstr dynstr_resize(dynstr s, size_t size);

/**
 * Set dynamic string size to zero.
 *
 * Guaranteed to not trigger reallocation.
 */
void dynstr_clear(dynstr s);

/** Assign dynamic string from a character array. */
dynstr dynstr_set_chars(dynstr s, const char *data, size_t count);

/** Assign dynamic string form a NULL-terminated C string. */
dynstr dynstr_set(dynstr s, const char *data);

/** Copy one dynamic string into another. */
dynstr dynstr_copy(dynstr dest, dynstr src);

/**
 * Append a single character \p c` to dynamic string \p s.
 */
dynstr dynstr_push(dynstr s, int c);

/**
 * Append a character array \p chars to a dynamic string.
 */
dynstr dynstr_push_chars(dynstr s, const char *chars, size_t count);

/**
 * Append a NULL-terminated C-string \p s to a dynamic string \p dest.
 */
dynstr dynstr_append(dynstr dest, const char *s);

/**
 * Append a dynamic string \p src to the end of a dynamic string \p dest
 */
dynstr dynstr_cat(dynstr dest, dynstr src);

/**
 * Append printf-formatted contents to a dynamic string \p dest.
 */
dynstr dynstr_printf(dynstr dest, DYNSTR_FMT_ARG const char *fmt, ...) DYNSTR_FMT_FUNC(2, 3);

/**
 * Append printf-formatted contents to a dynamic string \p dest using `va_list` arguments.
 */
dynstr dynstr_vprintf(dynstr dest, const char *fmt, va_list args);

/**
 * In-place takes a substring of dynamic string.
 *
 * Both arguments can be negative (in this case they counted relatively to the string end).
 */
void dynstr_range(dynstr s, ptrdiff_t start, ptrdiff_t count);

/**
 * In-place trims first characters from a provided character set (NULL-terminated).
 */
void dynstr_trim_start(dynstr s, const char *chars);

/**
 * In-place trims last characters from a provided character set (NULL-terminated).
 */
void dynstr_trim_end(dynstr s, const char *chars);

/**
 * In-place trims first and last characters from a provided character set (NULL-terminated).
 */
void dynstr_trim(dynstr s, const char *chars);

/**
 * Compare two dynamic strings.
 * Return value has the same meaning as `strcmp`
 * (0 - \p a equal to \p b, -1 - \p a less than \p b, 1 - \p a greater than \p b).
 */
int dynstr_cmp(dynstr a, dynstr b);

/**
 * Join multiple dynamic strings into one using provided separator.
 *
 * \p if \p dest is NULL, a new string will be allocated, otherwise new strings will be appended to the existing one
 */
dynstr dynstr_join(dynstr dest, size_t count, dynstr *src, const char *separator);

/**
 * Frees multiple dynamic strings (some of them possible to be NULL).
 */
void dynstr_free_array(size_t count, dynstr *strings);

/**
 * Verify that a dynamic string has prefix \p prefix.
 */
bool dynstr_has_prefix(dynstr s, const char *prefix);

/**
 * Verify that a dynamic string has suffix \p suffix.
 */
bool dynstr_has_suffix(dynstr s, const char *suffix);
