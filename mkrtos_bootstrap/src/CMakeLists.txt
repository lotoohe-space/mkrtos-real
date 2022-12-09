cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.S *.S **/*.C *.c)

add_library(boot_src STATIC ${deps})
target_include_directories(
    boot_src
    PUBLIC
)
