cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# message("deps = ${deps}")

add_library(dietlibc_libcompat OBJECT ${deps})

target_include_directories(
    dietlibc_libcompat
    PUBLIC
    include_directories(${CMAKE_CURRENT_LIST_DIR}/)
)
