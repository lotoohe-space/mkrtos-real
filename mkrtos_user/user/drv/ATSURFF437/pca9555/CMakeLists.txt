cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 ")

file(
    GLOB deps
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/src/*.c
    *.c
)

add_executable(
    pca9555.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    pca9555.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    mk_drv
    mk_pca9555
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
    pca9555.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/device_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/core_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/project/at_surf_f437_board/
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/middlewares/pca9555_application_library
    ${CMAKE_CURRENT_SOURCE_DIR}/

    # ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pca9555
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_i2c
)
add_dependencies(
    pca9555.elf
    ${START_LIB}
    sys
    sys_util
    mk_char
    mk_drv
    mk_pca9555
)
set_target_properties(
    pca9555.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    pca9555_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S pca9555.elf > ${CMAKE_SOURCE_DIR}/build/output/pca9555.S
    COMMAND
    ${CMAKE_READELF} -a pca9555.elf > ${CMAKE_SOURCE_DIR}/build/output/pca9555.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S pca9555.elf pca9555.bin
    COMMAND
    ${CMAKE_SIZE} pca9555.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy pca9555.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/pca9555
    COMMAND
    cp pca9555.elf ${CMAKE_SOURCE_DIR}/build/output/pca9555.elf
)

add_dependencies(pca9555_dump pca9555.elf)
 