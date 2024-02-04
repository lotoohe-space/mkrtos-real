cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)

add_library(drv STATIC ${deps})
include_directories(
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/${CONFIG_CPU_TYPE}
)
if (${CONFIG_CPU_TYPE} STREQUAL "stm32f1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport
    )
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f2" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include
    )
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f4" )
    # if(${CONFIG_BOARD_NAME} STREQUAL "STM32F407VET6" )
    #     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F40_41xxx ")
    # endif()
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
    )
elseif(${CONFIG_CPU_TYPE} STREQUAL "swm34s" )
    include_directories(

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DeviceSupport
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/CoreSupport
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver
    )
endif()
add_subdirectory(${CONFIG_CPU_TYPE})

target_link_libraries(
    drv
    PUBLIC
    knl_drv
    knl_bsp
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
add_dependencies(drv knl_bsp)
add_dependencies(drv arch)
