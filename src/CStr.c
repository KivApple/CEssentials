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
#include <stdlib.h>
#include <stdio.h>
#include "CStr.h"

typedef struct CStrHeader {
	size_t size;
	size_t capacity;
} CStrHeader;

static inline CStrHeader *CStr_header(CStr s) {
	return ((CStrHeader*) s) - 1;
}

static inline CStr CStr_alloc(size_t capacity) {
	CStrHeader *header = malloc(sizeof(CStrHeader) + capacity + 1);
	if (!header) return NULL;
	header->capacity = capacity;
	return (CStr) (header + 1);
}

CStr CStr_newCapacity(size_t capacity) {
	CStr s = CStr_alloc(capacity);
	if (!s) return NULL;
	CStr_header(s)->size = 0;
	s[0] = '\0';
	return s;
}

CStr CStr_newSize(const char *data, size_t size) {
	CStr s = CStr_alloc(size);
	if (!s) return NULL;
	CStr_header(s)->size = size;
	if (data) {
		memcpy(s, data, size);
	}
	s[size] = '\0';
	return s;
}

CStr CStr_dup(CStr s) {
	return CStr_newSize(s, CStr_header(s)->size);
}

void CStr_free(CStr s) {
	if (s) {
		free(CStr_header(s));
	}
}

size_t CStr_size(CStr s) {
	return CStr_header(s)->size;
}

size_t CStr_capacity(CStr s) {
	return CStr_header(s)->capacity;
}

CStr CStr_reserve(CStr s, size_t newCapacity) {
	if (CStr_header(s)->capacity >= newCapacity) return s;
	CStrHeader *newHeader = realloc(CStr_header(s), sizeof(CStrHeader) + newCapacity + 1);
	if (newHeader) {
		newHeader->capacity = newCapacity;
		return (CStr) (newHeader + 1);
	} else {
		free(s);
		return NULL;
	}
}

CStr CStr_resize(CStr s, size_t newSize) {
	s = CStr_reserve(s, newSize);
	if (!s) return NULL;
	CStr_header(s)->size = newSize;
	s[newSize] = '\0';
	return s;
}

void CStr_clear(CStr s) {
	CStr_header(s)->size = 0;
	s[0] = '\0';
}

CStr CStr_copyN(CStr dest, const char *data, size_t size) {
	dest = CStr_resize(dest, size);
	if (!dest) return NULL;
	memcpy(dest, data, size);
	return dest;
}

CStr CStr_copyCStr(CStr dest, CStr src) {
	return CStr_copyN(dest, src, CStr_header(src)->size);
}

CStr CStr_push(CStr s, char c) {
	if (CStr_header(s)->size == CStr_header(s)->capacity) {
		size_t newCapacity = CStr_header(s)->capacity ? CStr_header(s)->capacity * 2 : 16;
		s = CStr_reserve(s, newCapacity);
		if (!s) return NULL;
	}
	s[CStr_header(s)->size++] = c;
	s[CStr_header(s)->size] = '\0';
	return s;
}

CStr CStr_appendN(CStr s, const char *data, size_t size) {
	if (CStr_header(s)->size + size >= CStr_header(s)->capacity) {
		size_t newCapacity = CStr_header(s)->capacity ? CStr_header(s)->capacity * 2 : 16;
		while (newCapacity < CStr_header(s)->size + size) {
			newCapacity *= 2;
		}
		s = CStr_reserve(s, newCapacity);
		if (!s) return NULL;
	}
	memcpy(s + CStr_header(s)->size, data, size);
	CStr_header(s)->size += size;
	s[CStr_header(s)->size] = '\0';
	return s;
}

CStr CStr_appendCStr(CStr s, CStr data) {
	return CStr_appendN(s, data, CStr_header(data)->size);
}

CStr CStr_newFormatV(const char *fmt, va_list args) {
	va_list argsCopy;
	va_copy(argsCopy, args);
	int count = vsnprintf(NULL, 0, fmt, argsCopy);
	va_end(argsCopy);
	if (count < 0) {
		return CStr_new(fmt);
	}
	CStr s = CStr_alloc((size_t) count);
	if (!s) return NULL;
	vsnprintf(s, count + 1, fmt, args);
	CStr_header(s)->size = (size_t) count;
	return s;
}

CStr CStr_newFormat(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	CStr s = CStr_newFormatV(fmt, args);
	va_end(args);
	return s;
}

CStr CStr_appendFormatV(CStr s, const char *fmt, va_list args) {
	va_list argsCopy;
	va_copy(argsCopy, args);
	int count = vsnprintf(NULL, 0, fmt, argsCopy);
	va_end(argsCopy);
	if (count < 0) {
		return CStr_append(s, fmt);
	}
	if (CStr_header(s)->size + count > CStr_header(s)->capacity) {
		size_t newCapacity = CStr_header(s)->capacity ? CStr_header(s)->capacity * 2 : 16;
		while (newCapacity < CStr_header(s)->size + count) {
			newCapacity *= 2;
		}
		s = CStr_reserve(s, newCapacity);
		if (!s) return NULL;
	}
	vsnprintf(s + CStr_header(s)->size, count + 1, fmt, args);
	CStr_header(s)->size += (size_t) count;
	return s;
}

CStr CStr_appendFormat(CStr s, CSTR_FMT_ARG const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	s = CStr_appendFormatV(s, fmt, args);
	va_end(args);
	return s;
}

int CStr_cmpN(CStr a, const char *b, size_t size) {
	size_t minSize = CStr_header(a)->size;
	if (minSize > size) {
		minSize = size;
	}
	int result = memcmp(a, b, minSize);
	if (result == 0 && CStr_header(a)->size != size) {
		return CStr_header(a)->size > size ? 1 : -1;
	}
	return result;
}

int CStr_cmpCStr(CStr a, CStr b) {
	return CStr_cmpN(a, b, CStr_header(b)->size);
}

const char *CStr_chr(CStr s, int c) {
	return memchr(s, c, CStr_header(s)->size);
}

const char *CStr_rchr(CStr s, int c) {
	for (ptrdiff_t i = (ptrdiff_t) CStr_header(s)->size - 1; i >= 0; i--) {
		if (s[i] == c) {
			return s + i;
		}
	}
	return NULL;
}

const char *CStr_strN(CStr s, const char *sub, size_t subSize) {
	return strnstr(s, CStr_header(s)->size, sub, subSize);
}

const char *CStr_cStr(CStr s, CStr sub) {
	return CStr_strN(s, sub, CStr_header(sub)->size);
}

void CStr_range(CStr s, ptrdiff_t begin, ptrdiff_t end) {
	ptrdiff_t size = (ptrdiff_t) CStr_size(s);
	if (!size) return;
	if (begin < 0) begin = size + begin;
	if (end < 0) end = size + end;
	if (begin > size) begin = size;
	if (begin < 0) begin = 0;
	if (end > size) end = size;
	if (end < 0) end = 0;
	size_t newSize = begin <= end ? 1 + end - begin : 0;
	memmove(s, s + begin, newSize);
	CStr_header(s)->size = newSize;
	s[newSize] = '\0';
}

void CStr_trimStartN(CStr s, const char *chars, size_t count) {
	size_t i = 0;
	while (i < CStr_size(s) && memchr(chars, s[i], count) != NULL) i++;
	if (i < CStr_size(s)) {
		if (i > 0) {
			memmove(s, s + i, CStr_size(s) - i);
		}
	}
	size_t newSize = CStr_size(s) - i;
	CStr_header(s)->size = newSize;
	s[newSize] = '\0';
}

void CStr_trimEndN(CStr s, const char *chars, size_t count) {
	size_t i = CStr_size(s);
	while (i > 0 && memchr(chars, s[i - 1], count) != NULL) i--;
	CStr_header(s)->size = i;
	s[i] = '\0';
}

void CStr_trimN(CStr s, const char *chars, size_t count) {
	size_t begin = 0;
	while (begin < CStr_size(s) && memchr(chars, s[begin], count) != NULL) begin++;
	size_t end = CStr_size(s);
	while (end > begin && memchr(chars, s[end - 1], count) != NULL) end--;
	CStr_range(s, (ptrdiff_t) begin, (ptrdiff_t) end - 1);
}

bool CStr_startsWithN(CStr s, const char *sub, size_t subSize) {
	if (CStr_size(s) >= subSize) {
		return memcmp(s, sub, subSize) == 0;
	} else {
		return false;
	}
}

bool CStr_startsWithCStr(CStr s, CStr sub) {
	return CStr_startsWithN(s, sub, CStr_size(sub));
}

bool CStr_endsWithN(CStr s, const char *sub, size_t subSize) {
	size_t size = CStr_size(s);
	if (size >= subSize) {
		return memcmp(s + size - subSize, sub, subSize) == 0;
	} else {
		return false;
	}
}

bool CStr_endsWithCStr(CStr s, CStr sub) {
	return CStr_endsWithN(s, sub, CStr_size(sub));
}

const char *strnstr(const char *s, size_t size, const char *sub, size_t subSize) {
	if (!subSize) {
		return s;
	}
	if (subSize <= size) {
		for (size_t i = 0; i < size + 1 - subSize; i++) {
			if (s[i] == *sub) {
				if (memcmp(s + i, sub, subSize) == 0) {
					return s + i;
				}
			}
		}
	}
	return NULL;
}
