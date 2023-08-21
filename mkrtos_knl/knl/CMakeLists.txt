cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps *.c *.S)

add_library(knl STATIC ${deps})
target_include_directories(
    knl
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
)
