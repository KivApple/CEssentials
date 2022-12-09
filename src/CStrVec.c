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
#include "CStrVec.h"

void CStrVec_clearRecursive(CStrVec *v) {
	for (size_t i = 0; i < CStrVec_size(v); i++) {
		CStr_free(*CStrVec_at(v, i));
	}
	CStrVec_clear(v);
}

void CStrVec_destroyRecursive(CStrVec *v) {
	for (size_t i = 0; i < CStrVec_size(v); i++) {
		CStr_free(*CStrVec_at(v, i));
	}
	CStrVec_destroy(v);
}

bool CStr_splitN(CStr s, const char *separator, size_t separatorSize, int limit, CStrVec *out) {
	const char *begin = s, *end;
	while (
			(limit <= 0 || CStrVec_size(out) < limit - 1) &&
					(end = strnstr(begin, CStr_size(s), separator, separatorSize)) != NULL
			) {
		CStr sub = CStr_newSize(begin, end - begin);
		if (!sub) return false;
		if (!CStrVec_push(out, &sub)) return false;
		begin = end + separatorSize;
	}
	CStr sub = CStr_newSize(begin, s + CStr_size(s) - begin);
	if (!sub) return false;
	if (!CStrVec_push(out, &sub)) return false;
	return true;
}

bool CStr_splitCStr(CStr s, CStr separator, int limit, CStrVec *out) {
	return CStr_splitN(s, separator, CStr_size(separator), limit, out);
}

CStr CStrVec_joinN(CStrVec *v, const char *separator, size_t separatorSize, CStr dest) {
	size_t totalSize = 0;
	if (CStrVec_size(v) > 0) {
		for (size_t i = 0; i < CStrVec_size(v); i++) {
			totalSize += CStr_size(*CStrVec_at(v, i));
		}
		totalSize += separatorSize * (CStrVec_size(v) - 1);
	}
	if (!dest) {
		dest = CStr_newCapacity(totalSize);
	} else {
		dest = CStr_reserve(dest, CStr_size(dest) + totalSize);
	}
	if (!dest) return NULL;
	for (size_t i = 0; i < CStrVec_size(v); i++) {
		if (i > 0) {
			dest = CStr_appendN(dest, separator, separatorSize);
		}
		dest = CStr_appendCStr(dest, *CStrVec_at(v, i));
	}
	return dest;
}

CStr CStrVec_joinCStr(CStrVec *v, CStr separator, CStr dest) {
	return CStrVec_joinN(v, separator, CStr_size(separator), dest);
}
