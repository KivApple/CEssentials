cmake_minimum_required(VERSION 3.1)
project(CEssentials C)

set(CMAKE_C_STANDARD 11)

find_package(Doxygen)

add_library(CEssentials STATIC src/dynstr.c src/dynstrsplit.c)
target_include_directories(CEssentials PUBLIC include)

add_library(CEssentials::CEssentials ALIAS CEssentials)

if(DOXYGEN_FOUND)
	set(DOXYGEN_DISABLE_INDEX TRUE)
	doxygen_add_docs(CEssentials_docs CEssentials WORKING_DIRECTORY include)
endif()

if(CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
	add_executable(
			CEssentials_test
			test/main.c
			test/test_dynstr.c
			test/test_dynvec.c
			test/test_dynstrsplit.c
			test/test_hashtable.c
			test/test_hashset.c
			test/test_qsort.c
	)
	target_link_libraries(CEssentials_test CEssentials::CEssentials)
endif()
