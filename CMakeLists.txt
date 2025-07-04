cmake_minimum_required(VERSION 3.20)
project(tiny_stl)

set(CXX_STANDARD 23)

set(BUILD "debug") # or "release"

if(BUILD STREQUAL "debug")
    add_compile_options(-g -O0)
elseif(BUILD STREQUAL "release")
    add_compile_options(-O3)
endif()

aux_source_directory(./test TESTS)
include_directories(./include)

add_executable(test ${TESTS})

