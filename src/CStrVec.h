#pragma once
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

#include "CVec.h"
#include "CStr.h"

CVec_TYPEDEF(CStrVec, CStr)
CVec_IMPL(CStrVec, CStr)

void CStrVec_clearRecursive(CStrVec *v);
void CStrVec_destroyRecursive(CStrVec *v);

bool CStr_splitN(CStr s, const char *separator, size_t separatorSize, int limit, CStrVec *out);
static inline bool CStr_split(CStr s, const char *separator, int limit, CStrVec *out) {
	return CStr_splitN(s, separator, strlen(separator), limit, out);
}
bool CStr_splitCStr(CStr s, CStr separator, int limit, CStrVec *out);

CStr CStrVec_joinN(CStrVec *v, const char *separator, size_t separatorSize, CStr dest);
static inline CStr CStrVec_join(CStrVec *v, const char *separator, CStr dest) {
	return CStrVec_joinN(v, separator, strlen(separator), dest);
}
CStr CStrVec_joinCStr(CStrVec *v, CStr separator, CStr dest);
