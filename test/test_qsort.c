/*
Copyright 2023 Ivan Kolesnikov <kiv.apple@gmail.com>
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
#include <CEssentials/qsort.h>
#include "test_qsort.h"

void test_qsort(void) {
	int arr[] = {5, 3, 2, -10};
	
	qsort_int(arr, 0); // Must not crash
	assert(arr[0] == 5);
	assert(arr[1] == 3);
	
	qsort_int(arr, 1); // Must not crash
	assert(arr[0] == 5);
	assert(arr[1] == 3);
	
	qsort_int(arr, 2);
	assert(arr[0] == 3);
	assert(arr[1] == 5);
	
	qsort_int(arr, sizeof(arr) / sizeof(int));
	assert(arr[0] == -10);
	assert(arr[1] == 2);
	assert(arr[2] == 3);
	assert(arr[3] == 5);
	
	const char *strArr[] = {"pear", "peach", "apple"};
	
	qsort_str_const(strArr, sizeof(strArr) / sizeof(const char*));
	assert(strcmp(strArr[0], "apple") == 0);
	assert(strcmp(strArr[1], "peach") == 0);
	assert(strcmp(strArr[2], "pear") == 0);
	
	printf("qsort.h passed all tests!\n");
}
