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
#include <CEssentials/hashtable.h>
#include "test_hashtable.h"

void test_hashtable(void) {
	size_t index = 0;
	int absent;
	HT(const char*, int) ht;
	ht_init(ht);
	
	ht_put_str(ht, int, "10", index, absent);
	assert(absent == 1);
	assert(ht_valid(ht, index));
	ht_value(ht, index) = 10;
	
	ht_put_str(ht, int, "20", index, absent);
	assert(absent == 1);
	assert(ht_valid(ht, index));
	ht_value(ht, index) = 20;
	
	assert(ht_capacity(ht) == 4);
	
	ht_get_str(ht, "10", index);
	assert(ht_valid(ht, index));
	assert(ht_value(ht, index) == 10);
	
	ht_put_str(ht, int, "30", index, absent);
	assert(absent == 1);
	assert(ht_valid(ht, index));
	ht_value(ht, index) = 30;
	
	ht_put_str(ht, int, "40", index, absent);
	assert(absent == 1);
	assert(ht_valid(ht, index));
	ht_value(ht, index) = 40;
	
	assert(ht_capacity(ht) > 4);
	
	int sum = 0;
	ht_for_each(ht, i) {
		sum += ht_value(ht, i);
	}
	assert(sum == 10 + 20 + 30 + 40);
	
	ht_put_str(ht, int, "20", index, absent);
	assert(absent == 0);
	assert(ht_valid(ht, index));
	ht_value(ht, index) = 20;
	
	ht_get_str(ht, "20", index);
	assert(ht_valid(ht, index));
	assert(ht_value(ht, index) == 20);
	
	ht_delete(ht, index);
	ht_get_str(ht, "20", index);
	assert(!ht_valid(ht, index));
	
	ht_put_str(ht, int, "20", index, absent);
	assert(absent == 1);
	assert(ht_valid(ht, index));
	ht_value(ht, index) = 20;
	
	ht_clear(ht);
	ht_get_str(ht, "10", index);
	assert(!ht_valid(ht, index));
	
	ht_destroy(ht);
	
	printf("hashtable.h passed all tests!\n");
}
