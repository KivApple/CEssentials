cmake_minimum_required(VERSION 3.1)
project(CEssentials C)

set(CMAKE_C_STANDARD 99)

add_library(CEssentials STATIC src/CStr.c src/CStrVec.c)
target_include_directories(CEssentials INTERFACE src)

if(CMAKE_CURRENT_BINARY_DIR STREQUAL CMAKE_BINARY_DIR)
	add_executable(CEssentials_test tst/main.c)
	target_link_libraries(CEssentials_test PRIVATE CEssentials)
endif()
