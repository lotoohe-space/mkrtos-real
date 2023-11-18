cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

add_library(dietlibc_builtins OBJECT ${deps})
# add_library(dietlibc_arm OBJECT ${SRC_FILES})
target_include_directories(
    dietlibc_builtins
    PUBLIC
    include_directories(${CMAKE_CURRENT_LIST_DIR}/)
)
