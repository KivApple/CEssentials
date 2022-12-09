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
#include <assert.h>
#include <CVec.h>
#include <CStr.h>
#include <CStrVec.h>
#include <CHashTable.h>

CVec_TYPEDEF(IntVec, int)
CVec_IMPL(IntVec, int)

static void test_IntVec(void) {
	IntVec v;
	IntVec_init(&v);
	
	IntVec_reserve(&v, 32);
	assert(v.capacity == 32);
	
	*IntVec_append(&v) = 10;
	assert(v.capacity == 32);
	assert(v.size == 1);
	assert(*IntVec_at(&v, 0) == 10);
	
	for (int i = 0; i < 32; i++) {
		*IntVec_append(&v) = i;
	}
	assert(v.capacity >= 33);
	assert(v.size == 33);
	for (int i = 0; i < 32; i++) {
		assert(*IntVec_at(&v, i + 1) == i);
	}
	
	IntVec_resize(&v, 128);
	assert(v.capacity >= 128);
	assert(v.size == 128);
	
	IntVec_clear(&v);
	assert(v.size == 0);
	
	IntVec_destroy(&v);
}

static void test_CStr(void) {
	CStr s = CStr_newFormat("%s=%s", "a", "b");
	assert(CStr_size(s) == 3);
	assert(CStr_cmp(s, "a=b") == 0);
	assert(strcmp(s, "a=b") == 0); // C string compatibility
	
	CStr_resize(s, 0);
	assert(CStr_size(s) == 0);
	assert(strlen(s) == 0); // C string compatibility
	for (int i = 0; i < 32; i++) {
		s = CStr_push(s, 'A');
	}
	assert(CStr_size(s) == 32);
	assert(CStr_capacity(s) >= 32);
	assert(strlen(s) == 32); // C string compatibility
	
	s = CStr_appendFormat(s, "%s=%s", "a", "b");
	assert(CStr_size(s) == 32 + 3);
	assert(CStr_capacity(s) >= 32 + 3);
	
	s = CStr_append(s, "XYZ");
	assert(CStr_size(s) == 32 + 3 + 3);
	assert(CStr_capacity(s) >= 32 + 3 + 3);
	
	const char *sub = CStr_str(s, "a=b");
	assert(strcmp(sub, "a=bXYZ") == 0);

	const char *c = CStr_chr(s, 'a');
	assert(c == sub);
	
	s = CStr_copy(s, "abc");
	assert(CStr_size(s) == 3);
	s = CStr_append(s, "edf");
	assert(CStr_size(s) == 6);
	
	CStr s2 = CStr_new("g");
	
	s = CStr_appendCStr(s, s2);
	assert(CStr_size(s) == 7);
	
	sub = CStr_cStr(s, s2);
	assert(sub - s == 6);
	
	s = CStr_copyCStr(s, s2);
	assert(CStr_size(s) == 1);
	assert(CStr_cmpCStr(s, s2) == 0);
	
	s = CStr_push(s, 'h');
	assert(CStr_cmpCStr(s, s2) > 0);
	assert(CStr_cmpCStr(s2, s) < 0);
	
	CStr_free(s2);
	
	CStr_free(s);
	
	s = CStr_newCapacity(10);
	assert(CStr_capacity(s) >= 10);
	CStr_free(s);
}

static void test_CStr_split(void) {
	CStrVec v;
	CStrVec_init(&v);
	
	CStr s = CStr_new("a==b==c==d");
	CStr_split(s, "==", 3, &v);
	assert(CStrVec_size(&v) == 3);
	assert(CStr_cmp(*CStrVec_at(&v, 0), "a") == 0);
	assert(CStr_cmp(*CStrVec_at(&v, 1), "b") == 0);
	assert(CStr_cmp(*CStrVec_at(&v, 2), "c==d") == 0);
	CStr_free(s);
	
	s = CStrVec_join(&v, "::", NULL);
	assert(CStr_cmp(s, "a::b::c==d") == 0);
	CStr_free(s);
	
	CStrVec_destroyRecursive(&v);
}

static void test_CStr_trim(void) {
	CStr s;
	
	// Trim start
	s = CStr_new("\r\ntest\r\n");
	CStr_trimStart(s, "\r\n");
	assert(CStr_cmp(s, "test\r\n") == 0);
	CStr_free(s);
	
	s = CStr_new("test\r\n");
	CStr_trimStart(s, "\r\n");
	assert(CStr_cmp(s, "test\r\n") == 0);
	CStr_free(s);
	
	s = CStr_new("");
	CStr_trimStart(s, "\r\n");
	assert(CStr_cmp(s, "") == 0);
	CStr_free(s);
	
	// Trim end
	s = CStr_new("\r\ntest\r\n");
	CStr_trimEnd(s, "\r\n");
	assert(CStr_cmp(s, "\r\ntest") == 0);
	CStr_free(s);
	
	s = CStr_new("\r\ntest");
	CStr_trimEnd(s, "\r\n");
	assert(CStr_cmp(s, "\r\ntest") == 0);
	CStr_free(s);
	
	s = CStr_new("");
	CStr_trimEnd(s, "\r\n");
	assert(CStr_cmp(s, "") == 0);
	CStr_free(s);
	
	// Trim
	s = CStr_new("\r\ntest\r\n");
	CStr_trim(s, "\r\n");
	assert(CStr_cmp(s, "test") == 0);
	CStr_free(s);
	
	s = CStr_new("test\r\n");
	CStr_trim(s, "\r\n");
	assert(CStr_cmp(s, "test") == 0);
	CStr_free(s);
	
	s = CStr_new("\r\ntest");
	CStr_trim(s, "\r\n");
	assert(CStr_cmp(s, "test") == 0);
	CStr_free(s);
	
	s = CStr_new("test");
	CStr_trim(s, "\r\n");
	assert(CStr_cmp(s, "test") == 0);
	CStr_free(s);
	
	s = CStr_new("");
	CStr_trim(s, "\r\n");
	assert(CStr_cmp(s, "") == 0);
	CStr_free(s);
}

static void test_CStr_startsEnds(void) {
	CStr s = CStr_new("Hello world");
	assert(CStr_startsWith(s, "Hello"));
	assert(CStr_endsWith(s, "world"));
	assert(!CStr_startsWith(s, "Hello world!"));
	assert(!CStr_endsWith(s, "Hello world!"));
	CStr_free(s);
}

CHashTable_TYPEDEF(StrIntHT, const char*, int)
CHashTable_IMPL(StrIntHT, const char*, int, true, CHashTable_strHash, CHashTable_strEq)

static void test_CHashTable(void) {
	StrIntHT table;
	StrIntHT_init(&table);
	
	int result;
	size_t i;
	
	i = StrIntHT_get(&table, "50");
	assert(!StrIntHT_valid(&table, i));
	
	i = StrIntHT_put(&table, "10", &result);
	assert(result == 1);
	assert(StrIntHT_totalCapacity(&table) > StrIntHT_capacity(&table));
	*StrIntHT_value(&table, i) = 10;
	
	i = StrIntHT_get(&table, "50");
	assert(!StrIntHT_valid(&table, i));
	
	i = StrIntHT_put(&table, "20", &result);
	assert(result == 1);
	assert(StrIntHT_totalCapacity(&table) > StrIntHT_capacity(&table));
	*StrIntHT_value(&table, i) = 20;
	
	i = StrIntHT_put(&table, "30", &result);
	assert(result == 1);
	assert(StrIntHT_totalCapacity(&table) > StrIntHT_capacity(&table));
	*StrIntHT_value(&table, i) = 30;
	
	i = StrIntHT_put(&table, "40", &result);
	assert(result == 1);
	assert(StrIntHT_totalCapacity(&table) > StrIntHT_capacity(&table));
	*StrIntHT_value(&table, i) = 40;
	
	assert(StrIntHT_size(&table) == 4);
	assert(StrIntHT_capacity(&table) >= 4);
	
	i = StrIntHT_get(&table, "10");
	assert(StrIntHT_valid(&table, i));
	assert(*StrIntHT_value(&table, i) == 10);
	
	i = StrIntHT_get(&table, "20");
	assert(StrIntHT_valid(&table, i));
	assert(*StrIntHT_value(&table, i) == 20);
	
	i = StrIntHT_get(&table, "30");
	assert(StrIntHT_valid(&table, i));
	assert(*StrIntHT_value(&table, i) == 30);
	
	i = StrIntHT_get(&table, "40");
	assert(StrIntHT_valid(&table, i));
	assert(*StrIntHT_value(&table, i) == 40);
	
	i = StrIntHT_put(&table, "10", &result);
	assert(result == 0);
	assert(StrIntHT_valid(&table, i));
	assert(*StrIntHT_value(&table, i) == 10);
	
	assert(StrIntHT_size(&table) == 4);
	assert(StrIntHT_capacity(&table) >= 4);
	
	i = StrIntHT_get(&table, "50");
	assert(!StrIntHT_valid(&table, i));
	
	i = StrIntHT_get(&table, "30");
	assert(StrIntHT_valid(&table, i));
	StrIntHT_delete(&table, i);
	
	i = StrIntHT_get(&table, "30");
	assert(!StrIntHT_valid(&table, i));
	
	i = StrIntHT_put(&table, "30", &result);
	assert(result == 1);
	assert(StrIntHT_valid(&table, i));
	
	StrIntHT_clear(&table);
	i = StrIntHT_get(&table, "10");
	assert(!StrIntHT_valid(&table, i));
	
	StrIntHT_destroy(&table);
}

int main() {
	test_IntVec();
	test_CStr();
	test_CStr_split();
	test_CStr_trim();
	test_CStr_startsEnds();
	test_CHashTable();
}
