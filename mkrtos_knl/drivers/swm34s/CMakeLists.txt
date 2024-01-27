cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ")
add_library(knl_drv STATIC ${deps})

target_include_directories(
    knl_drv
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
)
