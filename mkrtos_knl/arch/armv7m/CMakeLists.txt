cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)

add_library(arch STATIC ${deps})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 ")
if (${BOARD_NAME} STREQUAL "STM32F1x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/STM32F1x
    )
    add_subdirectory(stm32f1x)
elseif(${BOARD_NAME} STREQUAL "STM32F2x" )
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/stm32f2x/STM32F2xx_StdPeriph_Driver
    )
    add_subdirectory(stm32f2x)
elseif(${BOARD_NAME} STREQUAL "STM32F4x" )
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
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
)
