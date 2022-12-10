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
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <CEssentials/dynstrsplit.h>
#include "test_dynstrsplit.h"

void test_dynstrsplit(void) {
	dynstr_list_t strings;
	dynvec_init(strings);
	
	str_split("A==B==C", "==", 2, &strings);
	assert(dynvec_size(strings) == 2);
	assert(strcmp(dynvec_at(strings, 0), "A") == 0);
	assert(strcmp(dynvec_at(strings, 1), "B==C") == 0);
	
	dynstr s = dynstr_list_join(NULL, &strings, "::");
	assert(strcmp(s, "A::B==C") == 0);
	dynstr_free(s);
	
	dynstr_list_clear(&strings);
	dynvec_destroy(strings);
	
	printf("dynstrsplit.h passed all tests!\n");
}
