cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)
list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/link.lds.S)

add_library(arch STATIC ${deps})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 ")
message(=======${CONFIG_CPU_TYPE})
target_include_directories(
    arch
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/
)
if (${CONFIG_CPU_TYPE} STREQUAL "stm32f1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport
    )
    add_subdirectory(${CONFIG_CPU_TYPE})
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f2" )
# message(======="${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}")
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
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
