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
#include <CEssentials/dynvec.h>
#include "test_dynvec.h"

void test_dynvec(void) {
	dynvec(int) v;
	dynvec_init(v);
	
	dynvec_resize(v, 2, int);
	assert(dynvec_size(v) == 2);
	assert(dynvec_capacity(v) == 2);
	
	dynvec_clear(v);
	assert(dynvec_size(v) == 0);
	
	*dynvec_append(v, int) = 10;
	*dynvec_append(v, int) = 20;
	assert(dynvec_size(v) == 2);
	assert(dynvec_capacity(v) == 2);
	assert(dynvec_at(v, 0) == 10);
	assert(dynvec_at(v, 1) == 20);
	
	dynvec_push(v, int, 30);
	dynvec_push(v, int, 40);
	assert(dynvec_size(v) == 4);
	assert(dynvec_capacity(v) >= 4);
	assert(dynvec_at(v, 0) == 10);
	assert(dynvec_at(v, 1) == 20);
	assert(dynvec_at(v, 2) == 30);
	assert(dynvec_at(v, 3) == 40);
	
	int sum = 0;
	dynvec_for_each(v, i) {
		sum += dynvec_at(v, i);
	}
	assert(sum == 10 + 20 + 30 + 40);
	
	dynvec_destroy(v);
	
	printf("dynvec.h passed all tests!\n");
}
