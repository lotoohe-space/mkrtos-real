cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)

add_library(drv STATIC ${deps})
if (${BOARD_NAME} STREQUAL "STM32F1x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f1x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport
    )
    add_subdirectory(stm32f1x)
elseif(${BOARD_NAME} STREQUAL "STM32F2x" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f2x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include
    )
    add_subdirectory(stm32f2x)
elseif(${BOARD_NAME} STREQUAL "STM32F4x" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f4x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
    )
    add_subdirectory(stm32f4x)
endif()

target_link_libraries(
    drv
    PUBLIC
    knl_drv
    bsp
    arch
)

target_include_directories(
    drv
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
)
add_dependencies(drv bsp)
add_dependencies(drv arch)
