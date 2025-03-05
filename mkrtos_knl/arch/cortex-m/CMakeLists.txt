cmake_minimum_required(VERSION 3.13)

file(GLOB deps 
    ../cortex-m/arch.c
    ../cortex-m/atomic.S
    ../cortex-m/atomics.c
    ../cortex-m/core_it.c
    ../cortex-m/mm_space.c
    ../cortex-m/sche.S
    ../cortex-m/sched_arch.c
    ../cortex-m/syscall.S
    ../cortex-m/systick.c
    ../cortex-m/thread_armv7m.c
)
# list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/link.lds.S)

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
elseif (${CONFIG_CPU_TYPE} STREQUAL "stm32f4" )
    if(${BOARD} STREQUAL "STM32F407VET6" )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F40_41xxx ")
    endif()
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
    )
    add_subdirectory(${CONFIG_CPU_TYPE})
elseif(${CONFIG_CPU_TYPE} STREQUAL "at32f437" )
    if(${BOARD} STREQUAL "ATSURFF437" )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
    endif()
    target_include_directories(
        arch
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/inc
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
