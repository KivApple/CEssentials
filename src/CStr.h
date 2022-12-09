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

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#ifndef _MSC_VER
#define CSTR_FMT_FUNC(fmtIndex, firstArgIndex) __attribute__((format(printf, fmtIndex, firstArgIndex)))
#define CSTR_FMT_ARG
#else
#define CSTR_FMT_FUNC(fmtIndex, firstArgIndex)
#define CSTR_FMT_ARG _Printf_format_string_
#endif

typedef char *CStr;

CStr CStr_newCapacity(size_t capacity);
CStr CStr_newSize(const char *data, size_t size);
static inline CStr CStr_new(const char *data) {
	return CStr_newSize(data, strlen(data));
}
CStr CStr_dup(CStr s);
void CStr_free(CStr s);

size_t CStr_size(CStr s);
size_t CStr_capacity(CStr s);

CStr CStr_reserve(CStr s, size_t newCapacity);
CStr CStr_resize(CStr s, size_t newSize);
void CStr_clear(CStr s);

CStr CStr_copyN(CStr dest, const char *data, size_t size);
static inline CStr CStr_copy(CStr dest, const char *data) {
	return CStr_copyN(dest, data, strlen(data));
}
CStr CStr_copyCStr(CStr dest, CStr src);

CStr CStr_push(CStr s, char c);
CStr CStr_appendN(CStr s, const char *data, size_t size);
static inline CStr CStr_append(CStr s, const char *data) {
	return CStr_appendN(s, data, strlen(data));
}
CStr CStr_appendCStr(CStr s, CStr data);

CStr CStr_newFormatV(const char *fmt, va_list args);
CStr CStr_newFormat(CSTR_FMT_ARG const char *fmt, ...) CSTR_FMT_FUNC(1, 2);

CStr CStr_appendFormatV(CStr s, const char *fmt, va_list args);
CStr CStr_appendFormat(CStr s, CSTR_FMT_ARG const char *fmt, ...) CSTR_FMT_FUNC(2, 3);

int CStr_cmpN(CStr a, const char *b, size_t size);
static inline int CStr_cmp(CStr a, const char *b) {
	return CStr_cmpN(a, b, strlen(b));
}
int CStr_cmpCStr(CStr a, CStr b);

const char *CStr_chr(CStr s, int c);
const char *CStr_rchr(CStr s, int c);

const char *CStr_strN(CStr s, const char *sub, size_t subSize);
static inline const char *CStr_str(CStr s, const char *sub) {
	return CStr_strN(s, sub, strlen(sub));
}
const char *CStr_cStr(CStr s, CStr sub);

void CStr_range(CStr s, ptrdiff_t begin, ptrdiff_t end);

void CStr_trimStartN(CStr s, const char *chars, size_t count);
static inline void CStr_trimStart(CStr s, const char *chars) {
	CStr_trimStartN(s, chars, strlen(chars));
}

void CStr_trimEndN(CStr s, const char *chars, size_t count);
static inline void CStr_trimEnd(CStr s, const char *chars) {
	CStr_trimEndN(s, chars, strlen(chars));
}

void CStr_trimN(CStr s, const char *chars, size_t count);
static inline void CStr_trim(CStr s, const char *chars) {
	CStr_trimN(s, chars, strlen(chars));
}

bool CStr_startsWithN(CStr s, const char *sub, size_t subSize);
static inline bool CStr_startsWith(CStr s, const char *sub) {
	return CStr_startsWithN(s, sub, strlen(sub));
}
bool CStr_startsWithCStr(CStr s, CStr sub);

bool CStr_endsWithN(CStr s, const char *sub, size_t subSize);
static inline bool CStr_endsWith(CStr s, const char *sub) {
	return CStr_endsWithN(s, sub, strlen(sub));
}
bool CStr_endsWithCStr(CStr s, CStr sub);

const char *strnstr(const char *s, size_t size, const char *sub, size_t subSize);
