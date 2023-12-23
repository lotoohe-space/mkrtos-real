cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F40_41xxx ")
add_library(knl_drv STATIC ${deps})

target_include_directories(
    knl_drv
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
)
