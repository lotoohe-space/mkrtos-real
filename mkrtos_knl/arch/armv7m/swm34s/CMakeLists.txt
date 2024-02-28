cmake_minimum_required(VERSION 3.13)


file(GLOB deps **/*.s *.s **/*.C *.c)

file(GLOB bsp_src 
${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver/*.c
)
list(APPEND deps ${bsp_src})

add_library(knl_bsp STATIC ${deps})

target_include_directories(
    knl_bsp
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DeviceSupport
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/CoreSupport
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver
)
