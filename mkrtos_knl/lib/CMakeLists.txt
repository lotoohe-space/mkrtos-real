cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c)

add_library(lib STATIC ${deps})
target_include_directories(
    lib
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv

    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
)
