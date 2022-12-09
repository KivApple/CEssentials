# CEssentials

C11 library with some useful structures and functions 
that are missing from standard library.

## Requirements

- CMake 3.1+
- GCC (including MinGW), Clang or MSVC compiler 
  (probably other compilers with C11 support)

The build tested on Windows and Linux.

## Usage

Just add src directory contents to your project files.
There are no external dependencies except for C standard library.

You also can include the repository into your CMake project using
`add_subdirectory`. In this case `CEssentials` static library
will be available to your code.

## Contents

- CVec.h - generic vector container
- CStr.h - heap allocated string with many utilities
- CStrVec.h - string splitting and joining utilities
- CHashTable.h - generic hash table container with open addressing and
  quadratic probing

## CVec.h

Generic vector container (dynamic array).

Example:

    CVec_TYPEDEF(IntVec, int)
    CVec_IMPL(IntVec, int)
    ...
    IntVec v;
    IntVec_init(&v);
    int a = 10, b = 20;
    IntVec_push(&v, &a);
    IntVec_push(&v, &b);
    *IntVec_append(&v) = 30;
    int sum = 0;
    for (size_t i = 0; i < IntVec_size(&v); i++) {
        sum += *IntVec_at(&v, i);
    }
    printf("%i\n", sum); // Outputs "60"
    IntVec_destroy(&v);

This container doesn't use type erasure via `void*` to gain
the maximum performance, so you need to instantiate first all
type definitions and method implementations. Type definitions
and method implementations are coupled, so you can for example
define typedef in the include file (for example, because
you are using CVec inside your own struct) and define 
implementation only in your implementation file which actually
needs it.

All implementations methods are marked as `static inline`, so
the same implementation can be safely declared in a multiple 
translation units. Each method is prefixed by a vector type name 
and an underscore.

All elements in the vector are stored contiguously, so you
can use pointer arithmetic and `memcpy`, use the vector as a
read or write buffer until you respect its size. However,
you should consider the fact that some operations can cause
reallocation of the storage and in this case all previously
obtained pointers to the elements might become invalid
(the vector uses `realloc` internally for reallocation 
optimization, but there are no guaranties that it always can 
expand the memory region without moving it).

### API

`void name##_init(name *v);`

Initialize an empty vector. Doesn't perform any heap allocations.

`void name##_destroy(name *v);`

Destroy the vector. If there are heap allocated memory it will be
freed. If you store complex data inside the vector you might
have to destroy them first (e.g. free nested heap-allocated data 
structures) before calling this function to avoid a memory leak.

`size_t name##_size(const name *v);`

Returns the current size of the vector.

`size_t name##_capacity(const name *v)`

Returns the current capacity of the vector - how many elements it
can hold without reallocating the storage.

`element_type *name##_at(name *v, size_t i);`

Returns a pointer to the element at specified position. If `i` is
greater than current vector size the behavior is undefined.

`const element_type *name##_constAt(const name *v, size_t i);`

The same as previous one, but for constant vector pointer.

`void name##_clear(name *v);`

Clears the vector. Doesn't free underlying memory chunk,
so subsequent insertions will be faster. It the vector holds
some complex data structure (e.g. stores nested heap-allocated
pointer) you have to destroy them first before calling this
function to avoid a memory leak.

`bool name##_reserve(name *v, size_t newCapacity);`

Grows internal vector storage to be able to hold at least 
`newCapacity` elements (subsequent insertions won't cause 
reallocation until the vector size reaches this value).

If current vector capacity is greater than the given one,
does nothing.

If memory allocation fails, the vector remains
unchanged, but `false` returned and a caller can handle
out of memory error.

`bool name##_resize(name *v, size_t newSize);`

Increases vector capacity to `newSize` if it was smaller,
then changes the vector size. If vector becomes larger than
before, the contents of new elements are undefined, so might
need to initialize them depending on your needs.

May trigger reallocation. 

If memory allocation fails, the vector remains
unchanged, but `false` returned and a caller can handle
out of memory error.

`element_type *name##_append(name *v);`

Increases vector size by one and returns a pointer to the
new element. It contents is undefined, so you might need to
initialize it.

May perform memory reallocation. 

If memory allocation fails, the vector remains
unchanged, but `NULL` returned and a caller can handle
out of memory error.

`element_type *name##_push(name *v, const element_type *value);`

The same as above, but initializes new element using `memcpy`.

## CStr.h

*Depends on CStr.c*

Dynamically allocated mutable string type compatible with 
C-strings.

Defines `CStr` typedef which actually just an alias for `char*`.
Can be directly passed to C functions that expect NULL-terminated
`const char*` or `char*`. They can ever update a value,
but **shouldn't** change the length of the string or 
call `realloc` or `free` on it. All `CStr` instances
must be freed only by `CStr_free`.

`CStr` has a header that stored just before character data 
itself. This header holds two `size_t` - an actual string size 
and a memory block size (to support growth of the string 
without reallocation if possible). Character data are always
zero-terminated, but actually all `CStr` methods handles
NULL characters inside the string correctly, so you can handle
arbitrary data.

Example:

    CStr s = CStr_new("Hello");
    s = CStr_append(" world");
    printf("%s\n", s); // Use as a usual NULL-terminated string
    CStr_free(s);

Any method which can increase `CStr` size returns a new pointer.
You should use a new pointer, because `CStr` might be reallocated
and the previous pointer become invalid. In case of allocation
failure the return value will be `NULL` (but previous value will
be correctly freed, so there are no memory leak possible).

### API

`CStr CStr_newCapacity(size_t capacity);`

Allocate an empty `CStr` that can hold up to `capacity` 
characters without reallocation (automatically added NULL 
character is not counted).

`CStr CStr_newSize(const char *data, size_t size);`

Allocate a `CStr` with provided contents. The data can
include NULL characters, because the size is explicitly 
specified.

If `data` is NULL, then string just allocated with specific length,
but character values are undefined.

`CStr CStr_new(const char *data);`

Allocate a `CStr` with provided NULL-terminated contents.

`CStr CStr_dup(CStr s);`

Allocate a `CStr` with copy of contents of the existing `CStr`.

`void CStr_free(CStr s);`

Free a `CStr` instance. All `CStr` instances must be freed only
via this function.

`size_t CStr_size(CStr s);`

Returns the number of characters in the `CStr`. Constant time
complexity operation.

`size_t CStr_capacity(CStr s);`

Returns the maximum number of characters that `CStr` can store
without reallocation.

`CStr CStr_reserve(CStr s, size_t newCapacity);`

If `newCapacity` is greater than current, the storage will
be reallocation to hold at least `newCapacity` characters.

If `newCapacity` is smaller or equal to current, the function
does nothing.

`CStr CStr_resize(CStr s, size_t newSize);`

Sets `CStr` size (if the new size is bigger than current 
capacity causes reallocation).

If new size is bigger than current added character values
are undefined.

`void CStr_clear(CStr s);`

Sets `CStr` size to zero.

`CStr CStr_copyN(CStr dest, const char *data, size_t size);`

Sets `CStr` size to size and initializes characters with 
provided data.

Might cause reallocation if `CStr` capacity is not enough.

`CStr CStr_copy(CStr dest, const char *data);`

Sets `CStr` size and contents from provided NULL-terminated 
string.

Might cause reallocation if `CStr` capacity is not enough.

`CStr CStr_copyCStr(CStr dest, CStr src);`

Sets `CStr` size and contents from provided `CStr`.

`CStr CStr_push(CStr s, char c);`

Appends one character to `CStr`.

Might cause reallocation, however if happens it may
allocate some extra space, so next appending probably won't cause
reallocation.

`CStr CStr_appendN(CStr s, const char *data, size_t size);`

Appends provided character array to `CStr`.

Might cause reallocation, however if happens it may
allocate some extra space, so next appending probably won't cause
reallocation.

`CStr CStr_append(CStr s, const char *data);`

Appends provided NULL-terminated string to `CStr`.

Might cause reallocation, however if happens it may
allocate some extra space, so next appending probably won't cause
reallocation.

`CStr CStr_appendCStr(CStr s, CStr data);`

Appends provided `CStr` to `CStr`.

Might cause reallocation, however if happens it may
allocate some extra space, so next appending probably won't cause
reallocation.

`CStr CStr_newFormatV(const char *fmt, va_list args);`

Allocates a new `CStr` with contents of `vsnprintf` output.
Performs exactly one allocation.

`CStr CStr_newFormat(const char *fmt, ...);`

Allocates a new `CStr` with contents of `snprintf` output.
Performs exactly one allocation.

`CStr CStr_appendFormatV(CStr s, const char *fmt, va_list args);`

Appends `vsnprintf` output to provided `CStr`.

Might cause reallocation, but if it happens it will be only one
reallocation. If cause of reallocation it may allocate some 
extra space, so next appending probably won't cause reallocation.

`CStr CStr_appendFormat(CStr s, const char *fmt, ...);`

The same as above, but for `snprintf`.

`int CStr_cmpN(CStr a, const char *b, size_t size)`;

Compares `CStr` with a provided character array. The result
has the same meaning as for `strcmp`:

`a == b => result == 0`

`a > b => result > 0`

`a < b => result < 0`

If one string is a prefix of another it considered less than 
another.

`int CStr_cmp(CStr a, const char *b);`

The same as above, but compares `CStr` with a NULL-terminated
string.

`int CStr_cmpCStr(CStr a, CStr b);`

The same as above, but compares two `CStr`.

`const char *CStr_chr(CStr s, int c);`

Searches for a character inside `CStr`. Returns pointer to
the first occurrence or `NULL`.

`const char *CStr_rchr(CStr s, int c);`

The same as above, but returns last occurrence if any.

`const char *CStr_strN(CStr s, const char *sub, size_t subSize);`

Searches for a substring of specified length in `CStr`.
Returns pointer to the first occurrence or `NULL`.

`const char *CStr_str(CStr s, const char *sub);`

The same as above, but searches for NULL-terminated string.

`const char *CStr_cStr(CStr s, CStr sub);`

The same as above, but searches for another `CStr`.

`void CStr_trimStartN(CStr s, const char *chars, size_t count);`

Trim the beginning of the string removing characters from
the provided set.

`void CStr_trimStart(CStr s, const char *chars);`

The same as above, but the character set is defined by a
NULL-terminated string.

`void CStr_trimEndN(CStr s, const char *chars, size_t count);`

Trim the ending of the string removing characters from
the provided set.

`void CStr_trimEnd(CStr s, const char *chars);`

The same as above, but the character set is defined by a
NULL-terminated string.

`void CStr_trimN(CStr s, const char *chars, size_t count);`

Trim the beginning and the ending of the string 
removing characters from the provided set.

`void CStr_trim(CStr s, const char *chars);`

The same as above, but the character set is defined by a
NULL-terminated string.

`bool CStr_startsWithN(CStr s, const char *sub, size_t subSize);`

Check if `sub` is a prefix of `s`.

`bool CStr_startsWith(CStr s, const char *sub);`

The same as above, but `sub` is a NULL-terminated string.

`bool CStr_startsWithCStr(CStr s, CStr sub);`

The same as above, but `sub` is a `CStr`.

`bool CStr_endsWithN(CStr s, const char *sub, size_t subSize);`

Check if `sub` is a suffix of `s`.

`bool CStr_endsWith(CStr s, const char *sub);`

The same as above, but `sub` is a NULL-terminated string.

`bool CStr_endsWithCStr(CStr s, CStr sub);`

The same as above, but `sub` is a `CStr`.

## CStrVec.h

*Depends on CStr.h and CVec.h*

Contains vector of `CStr` declaration and implementation with
utility functions for string splitting and joining.

### API

`bool CStr_splitN(CStr s, const char *separator, size_t separatorSize, int limit, CStrVec *out);`

Performs splitting of `CStr` by a provided separator with specific 
size. The results are stored in provided `CStrVec` (`CVec` of 
`CStr`, must be freed using `CStrVec_destroyRecursive` to 
avoid a memory leak).

`limit` sets the maximum amount of resulting items. If
`limit` <= 0, then there are no limit.

On vector reallocation failure returns `false` (but vector still
can contain some elements which must be freed after usage).

`bool CStr_split(CStr s, const char *separator, int limit, CStrVec *out);`

The same as above, but for NULL-terminated string separator.

`bool CStr_splitCStr(CStr s, CStr separator, int limit, CStrVec *out);`

The same as above, but for `CStr` as a separator.

`CStr CStrVec_joinN(CStrVec *v, const char *separator, size_t separatorSize, CStr dest);`

Joins all `CStr` from provided vector using a provided separator
with specified size.

`dest` argument can be `NULL`, in this case a newly allocated
`CStr` is returned. Otherwise, the concatenation result will be
appended to `dest`. Might cause `dest` reallocation, but exactly
once.

`CStr CStrVec_join(CStrVec *v, const char *separator, CStr dest);`

The same as above, but a separator is a NULL-terminated string.

`CStr CStrVec_joinCStr(CStrVec *v, CStr separator, CStr dest);`

The same as above, but a separator is another `CStr`.

`void CStrVec_clearRecursive(CStrVec *v);`

Should be used instead of `CStrVec_clear` to avoid a memory leak,
except if you take ownership over all `CStr` inside the vector
and free them manually.

`void CStrVec_destroyRecursive(CStrVec *v);`

Should be used instead of `CStrVec_destroy` to avoid a memory
leak, except if you take ownership over all `CStr` inside 
the vector and free them manually.

## CHashTable.h

Generic hash table implementation using open addressing and 
quadratic probing.

Example:

    CHashTable_TYPEDEF(StrIntHT, const char*, int)
    CHashTable_IMPL(StrIntHT, const char*, int, true, CHashTable_strHash, CHashTable_strEq)
    ...
    StrIntHT table;
    int result;
    size_t i;
    StrIntHT_init(&table);
    
    i = StrIntHT_put(&table, "10", &result);
    *StrIntHT_value(&table, i) = 10;
    i = StrIntHT_put(&table, "20", &result);
    *StrIntHT_value(&table, i) = 20;
    i = StrIntHT_put(&table, "30", &result);
    *StrIntHT_value(&table, i) = 30;
    
    i = StrIntHT_get(&table, "20");
    if (StrIntHT_valid(&table, i)) {
        printf("%i\n", StrIntHT_value(&table, i));
    } else {
        printf("Not found\n");
    }
    
    StrIntHT_destroy(&table);

`CHashTable_TYPEDEF(name, key_type, value_type)`

Defines a type for hash table with specific key and value types.
In case that you are not going to store values, just put any
type in place of value_type (e.g. `char`). It won't affect
structure size.

`CHashTable_IMPL(name, key_type, value_type, has_values, hash_func, eq_func)`

`has_values` is `bool` which enables storing values (`map` mode)
instead of just storing keys (`set` mode).

`hash_func` - a function or macro definition that accepts
a single argument of `value_type` and returns `size_t` with
hash value for it.

`eq_func` - a function or macro definition that accepts two
arguments of `value_type` and returns `true` if they are equal and
`false` otherwise.

Note: if for given `a` and `b` `eq_func(a, b)` returns `true`, 
then the expression `hash_func(a) == hash_func(b)` must be also 
true.

Carefully pick `hash_func`, because hash collisions affect 
performance.

### API

`void name##_init(name *table);`

Initializes empty hash table. No memory allocations are performed.

`void name##_destroy(name *table);`

Destroys hash table. If you store complex keys or values you
might need to manually destroy them before calling this
functions (e.g. free nested heap allocated pointers).

`size_t name##_size(const name *table);`

Returns a number of elements inside the hash table.

`size_t name##_capacity(const name *table)`

Returns a number of elements that hash table currently can store
without growing and rehashing.

`size_t name##_totalCapacity(const name *table)`

Returns a total hash table size. Always greater than capacity.

`void name##_clear(name *table);`

Clear hash table. If you store complex keys or values you
might need to manually destroy them before calling this
functions (e.g. free nested heap allocated pointers).

All element indices become invalid after this operation.
Do not try to use them (even for validity checking).

`bool name##_reserve(name *table, size_t newCapacity);`

If `newCapacity` is greater than current hash table capacity,
grows hash table and perform rehashing. It is guaranteed
that after this function call the hash table can at least
`newCapacity` elements without reallocation.

If memory allocation failed, returns `false` and hash table
won't be changed.

`size_t name##_begin(const name *table);`

Returns the first index for iteration.

`inline size_t name##_end(const name *table);`

Returns the last index for iteration.

`bool name##_valid(const name *table, size_t i);`

Checks if the index is valid.

`key_type name##_key(const name *table, size_t i);`

Returns the key by the index (index must be valid 
otherwise the behavior is undefined).

`value_type *name##_value(name *table, size_t i);`

Returns the value by index (index must be valid and 
`has_values` must be `true` otherwise the behavior is undefined).

`size_t name##_get(const name *table, key_type key);`

Perform hash table lookup. The result must be checked for
validity by `name##_valid` and then can be accessed using
`name##_key` and `name##_value`.

`size_t name##_put(name *table, key_type key, int *result);`

Try to put the key inside the hash table. If the key was inserted,
`result` will be set to 1, if the key was already exist, `result`
will be set to 0. If rehashing was triggered and memory allocation
failed, `result` will be set to -1 and the hash table won't be
changed.

If `result` >= 0, the return value will be a valid index
which can be used with `name##_key` and `name##_value`.

If `result` < 0, any access attempt to the index including
validity check will cause undefined behavior.

If rehashing happens, all previously obtained indices
invalidated.

`void name##_delete(name *table, size_t i);`

Deletes the item from the hash table by its index. The index
will be invalidated, all other indices remains valid and no
rehashing can be performed.

### Iteration

If you need to iterate over all elements of the hash table,
you need to obtain first and last indices using begin and end
method and then access only valid elements:

    for (size_t i = StrIntHT_begin(&table); i != StrIntHT_end(&table); i++) {
        if (!StrIntHT_valid(&table, i)) continue; // Important
        printf("%s=%i\n", StrIntHT_key(&table, i), StrIntHT_value(&table, i));
    }

You can safely call `name##_delete` during iteration.

### Hash functions

There are few builtin hashing and equality functions:

`size_t CHashTable_intHash(int x);`

`bool CHashTable_intEq(int a, int b);`

Hash and equality functions for `int`. Just uses the value as it.
Might be not good for some input data.

`size_t CHashTable_strHash(const char *s);`

`bool CHashTable_strEq(const char *a, const char *b);`

Hash and equality functions for NULL-terminated strings.
