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
    set(LINKS_FLAGS " --gc-section ")
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
    set(LINKS_FLAGS " --gc-section ")

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
    set(LINKS_FLAGS " --gc-section ")

elseif(${CONFIG_CPU_TYPE} STREQUAL "Cortex-R52" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/Cortex-R52

    )
    add_subdirectory(bsp/Cortex-R52)
    set(LINKS_FLAGS " --gc-section ")

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
    set(LINKS_FLAGS " --gc-section ")

elseif(${CONFIG_CPU_TYPE} STREQUAL "aarch64_qemu" )
    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/bsp/AARCH64_QEMU
    )
    add_subdirectory(bsp/AARCH64_QEMU)
    set(LINKS_FLAGS " ")

endif()

if ((DEFINED CONFIG_MMU) AND (CONFIG_MMU STREQUAL "y"))
    add_custom_target(
        gen_sys_cpio

        COMMAND
        cd ${CMAKE_SOURCE_DIR}/build/output/cpio
        COMMAND
        ls | cpio -H newc -o > ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio
        COMMAND
        cd ${CMAKE_SOURCE_DIR}/build/output
        COMMAND
        ${CMAKE_OBJCOPY} -I binary -O elf64-littleaarch64 -B aarch64 rootfs.cpio rootfs.cpio.elf --rename-section .data=.cpio
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/output
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio.elf
        )
    add_dependencies(
        gen_sys_cpio
        init_dump_elf
        mkrtos_dump_elf
        sh_dump_elf
        cpiofs_dump_elf
        net_dump_elf
        hello_dump_elf
    )
    set_source_files_properties(${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio.elf PROPERTIES EXTERNAL_OBJECT true)
    add_executable(bootstrap.elf
        ${deps}
        ${CMAKE_SOURCE_DIR}/build/output/rootfs.cpio.elf
    )
else()
    add_executable(bootstrap.elf
    ${deps}
    )
endif()

# message("CONFIG_CPU_TYPE="${CONFIG_CPU_TYPE})
string(TOUPPER ${CONFIG_CPU_TYPE} cpu_type)
set_target_properties(bootstrap.elf PROPERTIES LINK_FLAGS 
"-T ${CMAKE_CURRENT_LIST_DIR}/bsp/${cpu_type}/link.lds ${LINKS_FLAGS} ")

target_link_libraries(
    bootstrap.elf
    --whole-archive
    boot_bsp
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)

add_custom_target(
    bootstrap_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S bootstrap.elf bootstrap.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp bootstrap.bin ${CMAKE_SOURCE_DIR}/build/output/bootstrap
    COMMAND
    cp bootstrap.elf ${CMAKE_SOURCE_DIR}/build/output/bootstrap.elf
    COMMAND
    ${CMAKE_SIZE} bootstrap.elf
)
add_dependencies(bootstrap_dump bootstrap.elf)
add_dependencies(bootstrap.elf boot_bsp)