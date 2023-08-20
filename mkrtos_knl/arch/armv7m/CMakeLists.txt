cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_library(arch OBJECT ${deps})
target_include_directories(
    arch
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/stm32f203/STM32F2xx_StdPeriph_Driver
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
)
