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
#include <CEssentials/hashset.h>
#include "test_hashset.h"

void test_hashset_overflow(void) {
	size_t index = 0;
	int absent;
	HS(const char*) hs;
	hs_init(hs);
	
	hs_put_str(hs, "10", index, absent);
	hs_put_str(hs, "20", index, absent);
	hs_put_str(hs, "30", index, absent);
	
	hs_get_str(hs, "10", index);
	hs_delete(hs, index);
	
	hs_put_str(hs, "40", index, absent);
	
	hs_get_str(hs, "10", index); // Will hang if there is a bug
	
	hs_destroy(hs);
}

void test_hashset(void) {
	size_t index = 0;
	int absent;
	HS(const char*) hs;
	hs_init(hs);
	
	hs_put_str(hs, "10", index, absent);
	assert(absent == 1);
	assert(hs_valid(hs, index));
	
	hs_put_str(hs, "20", index, absent);
	assert(absent == 1);
	assert(hs_valid(hs, index));
	
	assert(hs_capacity(hs) == 4);
	
	hs_get_str(hs, "10", index);
	assert(hs_valid(hs, index));
	
	hs_put_str(hs, "30", index, absent);
	assert(absent == 1);
	assert(hs_valid(hs, index));
	
	hs_put_str(hs, "40", index, absent);
	assert(absent == 1);
	assert(hs_valid(hs, index));
	
	assert(hs_capacity(hs) > 4);
	
	int sum = 0;
	hs_for_each(hs, i) {
		sum += atoi(hs_key(hs, i));
	}
	assert(sum == 10 + 20 + 30 + 40);
	
	hs_put_str(hs, "20", index, absent);
	assert(absent == 0);
	assert(hs_valid(hs, index));
	
	hs_get_str(hs, "20", index);
	assert(hs_valid(hs, index));
	
	hs_delete(hs, index);
	hs_get_str(hs, "20", index);
	assert(!hs_valid(hs, index));
	
	hs_put_str(hs, "20", index, absent);
	assert(absent == 1);
	assert(hs_valid(hs, index));
	
	hs_clear(hs);
	hs_get_str(hs, "10", index);
	assert(!hs_valid(hs, index));
	
	hs_destroy(hs);
	
	test_hashset_overflow();
	
	printf("hashset.h passed all tests!\n");
}
