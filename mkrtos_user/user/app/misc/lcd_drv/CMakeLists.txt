cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

file(GLOB bsp_src 
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver/*.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DSP_Src/ControllerFunctions/arm_sin_cos_f32.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DSP_Src/CommonTables/arm_common_tables.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DSP_Src/CommonTables/arm_const_structs.c
)
list(APPEND deps ${bsp_src})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ")

add_executable(lcd_drv.elf
    ${deps}
)
target_link_libraries(lcd_drv.elf
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
    lcd_drv.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/contrib/ports/mkrtos
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include/lwip/apps
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/lcd_drv/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/lcd_drv/xtinygui/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/printf/src/printf

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/lcd_drv/NES_Simulator

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DeviceSupport
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/CoreSupport
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/SWM341_StdPeriph_Driver

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/SWM34/SWM341_StdPeriph_Driver/CMSIS/DSP_Inc
)
add_dependencies(lcd_drv.elf
muslc
lwip
)
set_target_properties(lcd_drv.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
add_custom_target(
    lcd_drv_dump ALL
    COMMAND
    ${CMAKE_SIZE} lcd_drv.elf
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S lcd_drv.elf lcd_drv.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp lcd_drv.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/lcd_drv
    COMMAND
    cp lcd_drv.elf ${CMAKE_SOURCE_DIR}/build/output/lcd_drv.elf
)

add_dependencies(lcd_drv_dump lcd_drv.elf)
add_dependencies(lcd_drv_dump sys)
add_dependencies(lcd_drv_dump sys_util)
add_dependencies(lcd_drv_dump mr)
add_dependencies(lcd_drv_dump sys_svr)
add_dependencies(lcd_drv_dump start_tiny)
add_dependencies(lcd_drv_dump muslc)
add_dependencies(lcd_drv_dump printf)
 