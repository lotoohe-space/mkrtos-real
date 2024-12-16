cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")

file(
    GLOB deps
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/src/*.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/middlewares/i2c_application_library/i2c_application.c
    *.c
)


add_executable(
    i2c.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    i2c.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    mk_drv
    mk_i2c
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
    i2c.elf
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

    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_i2c
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
)
add_dependencies(
    i2c.elf
    ${START_LIB}
    sys
    sys_util
    mk_char
    mk_drv
    mk_i2c
)
set_target_properties(
    i2c.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    i2c_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S i2c.elf > ${CMAKE_SOURCE_DIR}/build/output/i2c.S
    COMMAND
    ${CMAKE_READELF} -a i2c.elf > ${CMAKE_SOURCE_DIR}/build/output/i2c.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S i2c.elf i2c.bin
    COMMAND
    ${CMAKE_SIZE} i2c.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy i2c.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/i2c
    COMMAND
    cp i2c.elf ${CMAKE_SOURCE_DIR}/build/output/i2c.elf
)

add_dependencies(i2c_dump i2c.elf)
 