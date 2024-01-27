cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Werror \
-DUSE_STDPERIPH_DRIVER=1 \
-DSMP=1 \
-Wno-unused-parameter \
-Wno-unused-function \
-Wno-unused-variable \
-Wno-deprecated \
")

include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc)

file(GLOB deps *.c)

if (${CONFIG_CPU_TYPE} STREQUAL "stm32f1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    
    file(GLOB 
    bsp_src
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/*.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/*.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport/*.c
    )
    list(APPEND deps ${bsp_src})
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/STM32F1

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport
    )
    add_subdirectory(bsp/STM32F1)
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f2" )

    file(GLOB bsp_src ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/*.c)
    list(APPEND deps ${bsp_src})
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/STM32F2

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include
    )
    add_subdirectory(bsp/STM32F2)
elseif(${CONFIG_CPU_TYPE} STREQUAL "stm32f4" )
    if(${CONFIG_BOARD_NAME} STREQUAL "STM32F407VET6" )
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F40_41xxx ")
    endif()

    file(GLOB bsp_src ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)
    list(APPEND deps ${bsp_src})

    list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c)
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/STM32F4

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Include
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include
    )
    add_subdirectory(bsp/STM32F4)
elseif(${CONFIG_CPU_TYPE} STREQUAL "Cortex-R52" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/Cortex-R52

    )
    add_subdirectory(bsp/Cortex-R52)
elseif(${CONFIG_CPU_TYPE} STREQUAL "swm34s" )

    file(GLOB bsp_src 
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DeviceSupport/system_SWM341.c
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver/*.c
    )
    list(APPEND deps ${bsp_src})

    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/SWM34S

        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DeviceSupport
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/CoreSupport
        ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver
    )
    add_subdirectory(bsp/SWM34S)
endif()

add_executable(bootstrap.elf
    ${deps}
)

set_target_properties(bootstrap.elf PROPERTIES LINK_FLAGS 
"-T ${CMAKE_CURRENT_LIST_DIR}/bsp/${CONFIG_CPU_TYPE}/link.lds --gc-section ")

target_link_libraries(
    bootstrap.elf
    # mk_bsp
    --whole-archive
    boot_bsp
    --no-whole-archive
)

add_custom_target(
    bootstrap_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S bootstrap.elf bootstrap.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    cp bootstrap.bin ${CMAKE_SOURCE_DIR}/build/output/bootstrap
    COMMAND
    cp bootstrap.bin ${CMAKE_SOURCE_DIR}/build/output/bootstrap.bin
    COMMAND
    cp bootstrap.elf ${CMAKE_SOURCE_DIR}/build/output/bootstrap.elf
    COMMAND
    ${CMAKE_SIZE} bootstrap.elf
)
add_dependencies(bootstrap_dump bootstrap.elf)
add_dependencies(bootstrap.elf boot_bsp)