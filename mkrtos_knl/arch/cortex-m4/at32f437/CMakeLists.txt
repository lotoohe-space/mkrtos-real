cmake_minimum_required(VERSION 3.13)


file(GLOB deps **/*.s *.s **/*.C *.c)

file(GLOB bsp_src ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/src/*.c)
list(APPEND deps ${bsp_src})

# list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c)
# message("knl_bsp:${deps}")
add_library(knl_bsp STATIC ${deps})

target_include_directories(
    knl_bsp
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/core_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/device_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/inc
)
