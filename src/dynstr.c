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
#include <string.h>
#include <stdio.h>
#include <CEssentials/roundup.h>
#include <CEssentials/dynstr.h>

#define dynstr_data(header) ((dynstr) ((header) + 1))

dynstr dynstr_alloc(size_t capacity) {
	size_t total_size = sizeof(dynstr_header_t) + capacity + 1;
	if (total_size <= sizeof(dynstr_header_t)) { // Integer overflow
		return NULL;
	}
	dynstr_header_t *header = malloc(total_size);
	if (!header) {
		return NULL;
	}
	header->capacity = capacity;
	header->size = 0;
	dynstr s = dynstr_data(header);
	s[0] = '\0';
	return s;
}

dynstr dynstr_new_chars(const char *data, size_t count) {
	dynstr s = dynstr_alloc(count);
	if (s) {
		dynstr_size(s) = count;
		if (data) {
			memcpy(s, data, count);
		}
		s[count] = '\0';
	}
	return s;
}

dynstr dynstr_new(const char *data) {
	return dynstr_new_chars(data, strlen(data));
}

dynstr dynstr_new_printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	dynstr s = dynstr_new_vprintf(fmt, args);
	va_end(args);
	return s;
}

dynstr dynstr_new_vprintf(const char *fmt, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	int count = vsnprintf(NULL, 0, fmt, args_copy);
	va_end(args_copy);
	if (count >= 0) {
		dynstr s = dynstr_new_chars(NULL, (size_t) count);
		vsnprintf(s, (size_t) count + 1, fmt, args);
		return s;
	} else {
		return dynstr_new(fmt);
	}
}

dynstr dynstr_dup(dynstr s) {
	return dynstr_new_chars(s, dynstr_size(s));
}

void dynstr_free(dynstr s) {
	if (!s) return;
	dynstr_header_t *header = dynstr_header(s);
	free(header);
}

dynstr dynstr_shrink(dynstr s) {
	dynstr_header_t *header = dynstr_header(s);
	if (header->size == header->capacity) return s;
	size_t total_size = sizeof(dynstr_header_t) + header->size + 1;
	dynstr_header_t *new_header = realloc(header, total_size);
	if (!new_header) {
		free(header);
		return NULL;
	}
	new_header->capacity = new_header->size;
	return dynstr_data(new_header);
}

dynstr dynstr_reserve(dynstr s, size_t capacity) {
	dynstr_header_t *header = dynstr_header(s);
	if (capacity <= header->capacity) return s;
	size_t new_total_size = sizeof(dynstr_header_t) + capacity + 1;
	if (new_total_size <= sizeof(dynstr_header_t)) { // Integer overflow
		free(header);
		return NULL;
	}
	dynstr_header_t *new_header = realloc(header, new_total_size);
	if (!new_header) {
		free(header);
		return NULL;
	}
	new_header->capacity = capacity;
	return dynstr_data(new_header);
}

dynstr dynstr_reserve2(dynstr s, size_t capacity) {
	if (capacity <= dynstr_capacity(s)) return s;
	roundupsize(capacity);
	return dynstr_reserve(s, capacity);
}

dynstr dynstr_resize(dynstr s, size_t size) {
	s = dynstr_reserve(s, size);
	if (s) {
		dynstr_size(s) = size;
		s[size] = '\0';
	}
	return s;
}

void dynstr_clear(dynstr s) {
	dynstr_size(s) = 0;
	s[0] = '\0';
}

dynstr dynstr_set_chars(dynstr s, const char *data, size_t count) {
	s = dynstr_resize(s, count);
	if (s) {
		memcpy(s, data, count);
	}
	return s;
}

dynstr dynstr_set(dynstr s, const char *data) {
	return dynstr_set_chars(s, data, strlen(data));
}

dynstr dynstr_copy(dynstr dest, dynstr src) {
	return dynstr_set_chars(dest, src, dynstr_size(src));
}

static inline dynstr dynstr_grow(dynstr s, size_t delta) {
	size_t new_capacity = dynstr_capacity(s) + delta;
	if (new_capacity < delta) {
		dynstr_free(s);
		return NULL;
	}
	return dynstr_reserve2(s, new_capacity);
}

dynstr dynstr_push(dynstr s, int c) {
	s = dynstr_grow(s, 1);
	if (s) {
		s[dynstr_size(s)++] = (char) c;
		s[dynstr_size(s)] = '\0';
	}
	return s;
}

dynstr dynstr_push_chars(dynstr s, const char *chars, size_t count) {
	s = dynstr_grow(s, count);
	if (s) {
		memcpy(s + dynstr_size(s), chars, count);
		dynstr_size(s) += count;
		s[dynstr_size(s)] = '\0';
	}
	return s;
}

dynstr dynstr_append(dynstr dest, const char *s) {
	return dynstr_push_chars(dest, s, strlen(s));
}

dynstr dynstr_cat(dynstr dest, dynstr src) {
	return dynstr_push_chars(dest, src, dynstr_size(src));
}

dynstr dynstr_printf(dynstr dest, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	dynstr s = dynstr_vprintf(dest, fmt, args);
	va_end(args);
	return s;
}

dynstr dynstr_vprintf(dynstr dest, const char *fmt, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	int count = vsnprintf(NULL, 0, fmt, args_copy);
	va_end(args_copy);
	if (count >= 0) {
		dest = dynstr_grow(dest, (size_t) count);
		if (dest) {
			vsnprintf(dest + dynstr_size(dest), (size_t) count + 1, fmt, args);
			dynstr_size(dest) += (size_t) count;
		}
	} else {
		dest = dynstr_append(dest, fmt);
	}
	return dest;
}

void dynstr_range(dynstr s, ptrdiff_t start, ptrdiff_t count) {
	size_t begin;
	if (start >= 0) {
		begin = (size_t) start;
	} else {
		begin = (size_t) -start;
		if (begin <= dynstr_size(s)) {
			begin = dynstr_size(s) - begin;
		} else {
			begin = 0;
		}
	}
	
	size_t end;
	if (count >= 0) {
		end = begin + (size_t) count;
		if (end < begin) { // Integer overflow
			end = dynstr_size(s);
		}
	} else {
		end = (size_t) -count;
		if (end <= dynstr_size(s)) {
			end = dynstr_size(s) - end;
		} else {
			end = 0;
		}
	}
	
	if (begin < end) {
		if (begin > 0) {
			memmove(s, s + begin, end - begin);
		}
		dynstr_size(s) = end - begin;
		s[dynstr_size(s)] = '\0';
	} else {
		dynstr_clear(s);
	}
}

void dynstr_trim_start(dynstr s, const char *chars) {
	size_t i = 0;
	while (i < dynstr_size(s) && strchr(chars, s[i]) != NULL) i++;
	if (i > 0) {
		memmove(s, s + i, dynstr_size(s) - i);
		dynstr_size(s) -= i;
		s[dynstr_size(s)] = '\0';
	}
}

void dynstr_trim_end(dynstr s, const char *chars) {
	size_t j = dynstr_size(s);
	while (j > 0 && strchr(chars, s[j - 1]) != NULL) j--;
	dynstr_size(s) = j;
	s[j] = '\0';
}

void dynstr_trim(dynstr s, const char *chars) {
	size_t i = 0;
	while (i < dynstr_size(s) && strchr(chars, s[i]) != NULL) i++;
	size_t j = dynstr_size(s);
	while (j > i && strchr(chars, s[j - 1]) != NULL) j--;
	if (j > i) {
		if (i > 0) {
			memmove(s, s + i, j - i);
		}
		dynstr_size(s) = j - i;
		s[dynstr_size(s)] = '\0';
	} else {
		dynstr_clear(s);
	}
}

int dynstr_cmp(dynstr a, dynstr b) {
	size_t a_size = dynstr_size(a);
	size_t b_size = dynstr_size(b);
	size_t min_size = a_size <= b_size ? a_size : b_size;
	int result = memcmp(a, b, min_size);
	if (result == 0) {
		if (a_size > b_size) {
			return 1;
		} else if (a_size < b_size) {
			return -1;
		} else {
			return 0;
		}
	} else {
		return result;
	}
}

dynstr dynstr_join(dynstr dest, size_t count, dynstr *src, const char *separator) {
	size_t separator_size = strlen(separator);
	size_t total_size = 0;
	for (size_t i = 0; i < count; i++) {
		if (i > 0) {
			total_size += separator_size;
			if (total_size < separator_size) { // Integer overflow
				dynstr_free(dest);
				return NULL;
			}
		}
		total_size += dynstr_size(src[i]);
		if (total_size < dynstr_size(src[i])) { // Integer overflow
			dynstr_free(dest);
			return NULL;
		}
	}
	if (dest) {
		total_size += dynstr_size(dest);
		if (total_size < dynstr_size(dest)) { // Integer overflow
			dynstr_free(dest);
			return NULL;
		}
		dest = dynstr_reserve(dest, total_size);
	} else {
		dest = dynstr_alloc(total_size);
	}
	if (!dest) {
		return NULL;
	}
	for (size_t i = 0; i < count; i++) {
		if (i > 0) {
			dest = dynstr_push_chars(dest, separator, separator_size);
			if (!dest) {
				return NULL;
			}
		}
		dest = dynstr_cat(dest, src[i]);
		if (!dest) {
			return NULL;
		}
	}
	return dest;
}

void dynstr_free_array(size_t count, dynstr *strings) {
	for (size_t i = 0; i < count; i++) {
		dynstr_free(strings[i]);
	}
}

bool dynstr_has_prefix(dynstr s, const char *prefix) {
	size_t prefix_size = strlen(prefix);
	if (prefix_size > dynstr_size(s)) {
		return false;
	}
	return memcmp(s, prefix, prefix_size) == 0;
}

bool dynstr_has_suffix(dynstr s, const char *suffix) {
	size_t suffix_size = strlen(suffix);
	if (suffix_size > dynstr_size(s)) {
		return false;
	}
	return memcmp(s + dynstr_size(s) - suffix_size, suffix, suffix_size) == 0;
}
