cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps **/*.C **/*.S **/*.s **/*.c *.s *.S *.C *.c)

add_library(kernel_test OBJECT ${deps})
target_include_directories(
    kernel_test
    PUBLIC
    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
