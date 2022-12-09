cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# message("deps = ${deps}")

add_library(dietlibc_libugly OBJECT ${deps})

target_include_directories(
    dietlibc_libugly
    PUBLIC
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
