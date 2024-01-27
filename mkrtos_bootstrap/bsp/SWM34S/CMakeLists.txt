cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps **/*.S *.S **/*.C *.c)

add_library(boot_bsp STATIC ${deps})
target_include_directories(
    boot_bsp
    PUBLIC
)
