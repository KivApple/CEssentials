# CEssentials

C11 library with some useful structures and functions 
that are missing from standard library.

## Requirements

- GCC (including MinGW), Clang or MSVC compiler 
  (probably other compilers with C11 support)
- CMake 3.1+ (optional)
- Doxygen (optional)

The build tested on Windows and Linux.

## Usage
Just add src and include directories contents to your project files.
There are no external dependencies except for the C standard library.

You also can include the repository into your CMake project 
using add_subdirectory. In this case CEssentials static 
library will be available to your code.

## Contents

- [dynstr.h](include/CEssentials/dynstr.h) - 
  Dynamic strings that are compatible with NULL terminated C-string.
- [dynstrsplit.h](include/CEssentials/dynstrsplit.h) -
  Splitting C-string by a separator into a `dynvec` of `dynstr`.
- [dynvec.h](include/CEssentials/dynvec.h) -
  Generic dynamic vector container.
- [hashtable.h](include/CEssentials/hashtable.h) -
  Generic hash table container with [quadratic probing](https://en.wikipedia.org/wiki/Quadratic_probing).

## LICENSE

MIT
