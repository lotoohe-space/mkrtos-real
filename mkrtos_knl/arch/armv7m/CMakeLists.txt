cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)

add_library(arch STATIC ${deps})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 ")
if (${BOARD_NAME} STREQUAL "STM32F1x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f1x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
    )
    add_subdirectory(stm32f1x)
elseif(${BOARD_NAME} STREQUAL "STM32F2x" )
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f2x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
        
    )
    add_subdirectory(stm32f2x)
elseif(${BOARD_NAME} STREQUAL "STM32F4x" )
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f4x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F40_41xxx ")
    add_subdirectory(stm32f4x)
endif()

target_link_libraries(
    arch
    PUBLIC
    knl_bsp
)
target_include_directories(
    arch
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
)
