cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")

file(
    GLOB deps
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/src/*.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/project/at_surf_f437_board/at_surf_f437_board_lcd.c
    *.c
)


add_executable(
    pin.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    pin.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    mk_drv
    mk_pin
    mk_char
    --whole-archive
    ${START_LIB}
    libc_be
    sys
    sys_util
    sys_svr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    pin.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/device_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/core_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/project/at_surf_f437_board/
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/middlewares/i2c_application_library
    ${CMAKE_CURRENT_SOURCE_DIR}/

    
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
)
add_dependencies(
    pin.elf
    ${START_LIB}
    sys
    sys_util
    mk_char
    mk_drv
    mk_pin
)
set_target_properties(
    pin.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    pin_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S pin.elf > ${CMAKE_SOURCE_DIR}/build/output/pin.S
    COMMAND
    ${CMAKE_READELF} -a pin.elf > ${CMAKE_SOURCE_DIR}/build/output/pin.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S pin.elf pin.bin
    COMMAND
    ${CMAKE_SIZE} pin.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy pin.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/pin
    COMMAND
    cp pin.elf ${CMAKE_SOURCE_DIR}/build/output/pin.elf
)

add_dependencies(pin_dump pin.elf)
 