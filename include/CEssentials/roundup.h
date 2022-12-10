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
 * @brief Round up to a nearest power of 2 macros
 */

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

/** Round up a 8-bit integer variable `x` to a nearest power of 2  */
#define roundup8(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, ++(x))

/** Round up a 16-bit integer variable `x` to a nearest power of 2  */
#define roundup16(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, ++(x))

/** Round up a 32-bit integer variable `x` to a nearest power of 2  */
#define roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

/** Round up a 64-bit integer variable `x` to a nearest power of 2  */
#define roundup64(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, (x)|=(x)>>32, ++(x))

#if SIZE_MAX == UINT64_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup64(x)
#elif SIZE_MAX == UINT32_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup32(x)
#elif SIZE_MAX == UINT16_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup16(x)
#elif SIZE_MAX == UINT8_MAX
/** Round up a size_t variable `x` to a nearest power of 2  */
#define roundupsize(x) roundup8(x)
#endif
