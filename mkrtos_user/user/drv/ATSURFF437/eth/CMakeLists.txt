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
    eth.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    eth.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    mk_drv
    mk_char
    --whole-archive
    ${START_LIB}
    libc_be
    sys
    sys_util
    sys_svr
    printf
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    eth.elf
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

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_eth
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pca9555
)
add_dependencies(
    eth.elf
    ${START_LIB}
    sys
    sys_util
    mk_char
    mk_drv
    printf
    # mk_eth
)
set_target_properties(
    eth.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    eth_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S eth.elf > ${CMAKE_SOURCE_DIR}/build/output/eth.S
    COMMAND
    ${CMAKE_READELF} -a eth.elf > ${CMAKE_SOURCE_DIR}/build/output/eth.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S eth.elf eth.bin
    COMMAND
    ${CMAKE_SIZE} eth.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy eth.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/eth
    COMMAND
    cp eth.elf ${CMAKE_SOURCE_DIR}/build/output/eth.elf
)

add_dependencies(eth_dump eth.elf)
 