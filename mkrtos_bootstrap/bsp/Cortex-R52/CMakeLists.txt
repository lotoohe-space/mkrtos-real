cmake_minimum_required(VERSION 3.13)


file(GLOB deps **/*.S *.S **/*.C *.c)

add_library(bsp STATIC ${deps})
target_include_directories(
    bsp
    PUBLIC
)
