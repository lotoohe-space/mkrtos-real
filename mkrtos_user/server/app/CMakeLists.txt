cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_XL\
")


add_executable(app.elf
    ${deps}
)
target_link_libraries(app.elf
    PUBLIC
    start
    muslc
    sys
    sys_util
    sys_svr
    xtinymodbus
    stm32f1_bsp
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    app.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/app/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/app/test

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/app/modbus
#xtinymodbus
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/modbus_base
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/modbus_rtu_master
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/modbus_rtu_slave
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/port
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/serial
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc
#end

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/app/hmi_lcd
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/app/util
)
add_dependencies(app.elf
muslc
)
set_target_properties(app.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
add_custom_target(
    app_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S app.elf > ${CMAKE_SOURCE_DIR}/build/output/app.S
    COMMAND
    ${CMAKE_READELF} -a app.elf > ${CMAKE_SOURCE_DIR}/build/output/app.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S app.elf app.bin
    COMMAND
    ${CMAKE_SIZE} app.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp app.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/app
    COMMAND
    cp app.elf ${CMAKE_SOURCE_DIR}/build/output/app.elf
)

add_dependencies(app_dump app.elf)
add_dependencies(app_dump sys)
add_dependencies(app_dump sys_util)
 