cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps *.c *.S)

if (NOT ${CONFIG_SMP} STREQUAL "y" )
    list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/buddy.c)
    list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/slab.c)
endif()

add_library(knl STATIC ${deps})

target_include_directories(
    knl
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/

    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv
)
