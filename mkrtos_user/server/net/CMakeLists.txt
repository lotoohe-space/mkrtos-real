cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

file(GLOB bsp_src ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)
list(APPEND deps ${bsp_src})
list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F40_41xxx\
")


add_executable(net.elf
    ${deps}
)
target_link_libraries(net.elf
    PUBLIC
    start
    muslc
    lwip
    sys
    sys_util
    sys_svr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    net.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/contrib/ports/mkrtos
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/net/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include


    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/STM32F4xx_StdPeriph_Driver/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0/Libraries/CMSIS/Device/ST/STM32F4xx/Include
)
add_dependencies(net.elf
muslc
lwip
)
set_target_properties(net.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
add_custom_target(
    net_dump ALL
    COMMAND
    ${CMAKE_SIZE} net.elf
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S net.elf net.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp net.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/net
    COMMAND
    cp net.elf ${CMAKE_SOURCE_DIR}/build/output/net.elf
)

add_dependencies(net_dump net.elf)
add_dependencies(net_dump sys)
add_dependencies(net_dump sys_util)
add_dependencies(net_dump mr)
add_dependencies(net_dump sys_svr)
add_dependencies(net_dump start)
add_dependencies(net_dump muslc)
add_dependencies(net_dump lwip)
 