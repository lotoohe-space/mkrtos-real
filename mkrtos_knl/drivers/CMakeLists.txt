cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)

add_library(drv STATIC ${deps})
if (${BOARD_NAME} STREQUAL "STM32F1x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/STM32F1x
    )
    add_subdirectory(stm32f1x)
elseif(${BOARD_NAME} STREQUAL "STM32F2x" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/stm32f203/STM32F2xx_StdPeriph_Driver
    )
    add_subdirectory(stm32f2x)
endif()

target_link_libraries(
    drv
    PUBLIC
    knl_drv
)

target_include_directories(
    drv
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m/stm32f203/STM32F2xx_StdPeriph_Driver
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc/armv7m
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
)
