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
#include <string.h>
#include <stdio.h>
#include <CEssentials/dynstr.h>
#include "test_dynstr.h"

static void test_dynstr_new(void) {
	dynstr s = dynstr_new("test");
	assert(dynstr_size(s) == 4);
	assert(strcmp(s, "test") == 0);
	dynstr_free(s);
}

static void test_dynstr_new_printf(void) {
	dynstr s = dynstr_new_printf("%s=%s", "a", "b");
	assert(dynstr_size(s) == 3);
	assert(strcmp(s, "a=b") == 0);
	dynstr_free(s);
}

static void test_dynstr_dup(void) {
	dynstr s1 = dynstr_new("test");
	dynstr s2 = dynstr_dup(s1);
	assert(s2 != s1);
	assert(strcmp(s1, s2) == 0);
	assert(dynstr_cmp(s1, s2) == 0);
	dynstr_free(s2);
	dynstr_free(s1);
}

static void test_dynstr_shrink(void) {
	dynstr s = dynstr_alloc(128);
	assert(dynstr_size(s) == 0);
	assert(dynstr_capacity(s) == 128);
	s = dynstr_shrink(s);
	assert(dynstr_capacity(s) == 0);
	dynstr_free(s);
}

static void test_dynstr_set(void) {
	dynstr s = dynstr_new("test");
	s = dynstr_set(s, "qwerty");
	assert(strcmp(s, "qwerty") == 0);
	dynstr_free(s);
}

static void test_dynstr_copy(void) {
	dynstr s1 = dynstr_new("test");
	dynstr s2 = dynstr_new("qwerty");
	assert(dynstr_cmp(s1, s2) != 0);
	s1 = dynstr_copy(s1, s2);
	assert(s1 != s2);
	assert(strcmp(s1, s2) == 0);
	dynstr_free(s2);
	dynstr_free(s1);
}

static void test_dynstr_push(void) {
	dynstr s = dynstr_new("abc");
	s = dynstr_push(s, 'd');
	assert(strcmp(s, "abcd") == 0);
	dynstr_free(s);
}

static void test_dynstr_append(void) {
	dynstr s = dynstr_new("abc");
	s = dynstr_append(s, "def");
	assert(strcmp(s, "abcdef") == 0);
	dynstr_free(s);
}

static void test_dynstr_cat(void) {
	dynstr s1 = dynstr_new("abc");
	dynstr s2 = dynstr_new("def");
	s1 = dynstr_append(s1, s2);
	assert(strcmp(s1, "abcdef") == 0);
	dynstr_free(s2);
	dynstr_free(s1);
}

static void test_dynstr_printf(void) {
	dynstr s = dynstr_new("abc");
	s = dynstr_printf(s, "%s=%s", "d", "f");
	assert(strcmp(s, "abcd=f") == 0);
	dynstr_free(s);
}

static void test_dynstr_range(void) {
	dynstr s = dynstr_new("");
	
	s = dynstr_set(s, "1234");
	dynstr_range(s, 0, dynstr_size(s));
	assert(strcmp(s, "1234") == 0);
	
	s = dynstr_set(s, "1234");
	dynstr_range(s, 1, dynstr_size(s) - 2);
	assert(strcmp(s, "23") == 0);
	
	s = dynstr_set(s, "1234");
	dynstr_range(s, 0, -1);
	assert(strcmp(s, "123") == 0);
	
	s = dynstr_set(s, "1234");
	dynstr_range(s, -1, 1);
	assert(strcmp(s, "4") == 0);
	
	dynstr_free(s);
}

static void test_dynstr_trim_start(void) {
	dynstr s = dynstr_new("\r\nabc");
	dynstr_trim_start(s, "\n\r");
	assert(strcmp(s, "abc") == 0);
	dynstr_trim_start(s, "\n\r");
	assert(strcmp(s, "abc") == 0);
	dynstr_free(s);
}

static void test_dynstr_trim_end(void) {
	dynstr s = dynstr_new("abc\r\n");
	dynstr_trim_end(s, "\n\r");
	assert(strcmp(s, "abc") == 0);
	dynstr_trim_end(s, "\n\r");
	assert(strcmp(s, "abc") == 0);
	dynstr_free(s);
}

static void test_dynstr_trim(void) {
	dynstr s = dynstr_new("\r\nabc\r\n");
	dynstr_trim(s, "\n\r");
	assert(strcmp(s, "abc") == 0);
	dynstr_trim(s, "\n\r");
	assert(strcmp(s, "abc") == 0);
	dynstr_free(s);
}

static void test_dynstr_has_prefix(void) {
	dynstr s = dynstr_new("abc");
	assert(dynstr_has_prefix(s, "a"));
	assert(dynstr_has_prefix(s, "ab"));
	assert(dynstr_has_prefix(s, "abc"));
	assert(!dynstr_has_prefix(s, "abcd"));
	assert(!dynstr_has_prefix(s, "b"));
	dynstr_free(s);
}

static void test_dynstr_has_suffix(void) {
	dynstr s = dynstr_new("abc");
	assert(dynstr_has_suffix(s, "c"));
	assert(dynstr_has_suffix(s, "bc"));
	assert(dynstr_has_suffix(s, "abc"));
	assert(!dynstr_has_suffix(s, "abcd"));
	assert(!dynstr_has_suffix(s, "e"));
	dynstr_free(s);
}

static void test_dynstr_join(void) {
	dynstr strings[] = {
			dynstr_new("Hello"),
			dynstr_new("world")
	};
	size_t count = sizeof(strings) / sizeof(strings[0]);
	dynstr s = dynstr_join(NULL, count, strings, " ");
	assert(strcmp(s, "Hello world") == 0);
	dynstr_free(s);
	dynstr_free_array(count, strings);
}

void test_dynstr(void) {
	test_dynstr_new();
	test_dynstr_new_printf();
	test_dynstr_dup();
	test_dynstr_shrink();
	test_dynstr_set();
	test_dynstr_copy();
	test_dynstr_push();
	test_dynstr_append();
	test_dynstr_cat();
	test_dynstr_printf();
	test_dynstr_range();
	test_dynstr_trim_start();
	test_dynstr_trim_end();
	test_dynstr_trim();
	test_dynstr_has_prefix();
	test_dynstr_has_suffix();
	test_dynstr_join();
	printf("dynstr.h passed all tests!\n");
}
