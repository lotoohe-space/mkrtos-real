cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps *.s *.S *.C *.c)
# list(APPEND deps ${bsp_src})

add_library(knl_bsp STATIC ${deps})
target_include_directories(
    knl_bsp
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/

)
