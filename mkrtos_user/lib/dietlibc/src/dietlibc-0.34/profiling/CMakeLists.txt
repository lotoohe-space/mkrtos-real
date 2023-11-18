cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# message("deps = ${deps}")

add_library(dietlibc_profiling OBJECT ${deps})

target_include_directories(
    dietlibc_profiling
    PUBLIC
    include_directories(${CMAKE_CURRENT_LIST_DIR}/)
)
