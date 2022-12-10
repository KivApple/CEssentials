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
#include <string.h>
#include <CEssentials/dynstrsplit.h>

bool str_split(const char *s, const char *separator, int max_split, dynstr_list_t *result) {
	size_t separator_size = strlen(separator);
	const char *begin = s, *end;
	while ((max_split <= 0 || dynvec_size(*result) < max_split - 1) && (end = strstr(begin, separator)) != NULL) {
		dynstr sub = dynstr_new_chars(begin, end - begin);
		if (!sub) {
			return false;
		}
		if (!dynvec_push(*result, dynstr, sub)) {
			return false;
		}
		begin = end + separator_size;
	}
	dynstr sub = dynstr_new(begin);
	if (!sub) {
		return false;
	}
	if (!dynvec_push(*result, dynstr, sub)) {
		return false;
	}
	return true;
}

dynstr dynstr_list_join(dynstr dest, dynstr_list_t *list, const char *separator) {
	return dynstr_join(dest, dynvec_size(*list), &dynvec_at(*list, 0), separator);
}

void dynstr_list_clear(dynstr_list_t *list) {
	dynvec_for_each(*list, i) {
		dynstr_free(dynvec_at(*list, i));
	}
	dynvec_clear(*list);
}
