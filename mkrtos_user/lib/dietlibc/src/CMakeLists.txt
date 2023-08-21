cmake_minimum_required(VERSION 3.13)


file(GLOB deps  *.c *.S)
add_subdirectory(dietlibc-0.34)

add_library(dietlibc_src STATIC ${deps})
target_include_directories(
    dietlibc_src
    PUBLIC
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)