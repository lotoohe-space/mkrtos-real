cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)

add_library(arch STATIC ${deps})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 ")
if (${CONFIG_CPU_TYPE} STREQUAL "stm32f1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f1
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport
    )
    add_subdirectory(stm32f1)
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f2" )
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/stm32f2
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
        
    )
    add_subdirectory(stm32f2)
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f4" )
    if(${BOARD} STREQUAL "STM32F407VET6" )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F40_41xxx ")
    endif()
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/armv7m/${CONFIG_CPU_TYPE}
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
    )
    add_subdirectory(${CONFIG_CPU_TYPE})
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
