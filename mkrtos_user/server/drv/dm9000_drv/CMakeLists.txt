cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

file(GLOB bsp_src 
${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/src/*.c
)
list(APPEND deps ${bsp_src})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_HD")

add_executable(dm9000_drv.elf
    ${deps}
)
target_link_libraries(dm9000_drv.elf
    PUBLIC
    start_tiny
    printf
    muslc
    sys
    sys_util
    sys_svr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    dm9000_drv.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/contrib/ports/mkrtos
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include/lwip/apps
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/dm9000_drv/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/printf/src/printf

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/STM32F10x_StdPeriph_Driver/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3/CoreSupport

)
add_dependencies(dm9000_drv.elf
muslc
lwip
)
set_target_properties(dm9000_drv.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
add_custom_target(
    dm9000_drv_dump ALL
    COMMAND
    ${CMAKE_SIZE} dm9000_drv.elf
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S dm9000_drv.elf dm9000_drv.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp dm9000_drv.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/dm9000_drv
    COMMAND
    cp dm9000_drv.elf ${CMAKE_SOURCE_DIR}/build/output/dm9000_drv.elf
)

add_dependencies(dm9000_drv_dump dm9000_drv.elf)
add_dependencies(dm9000_drv_dump sys)
add_dependencies(dm9000_drv_dump sys_util)
add_dependencies(dm9000_drv_dump mr)
add_dependencies(dm9000_drv_dump sys_svr)
add_dependencies(dm9000_drv_dump start_tiny)
add_dependencies(dm9000_drv_dump muslc)
add_dependencies(dm9000_drv_dump printf)
 