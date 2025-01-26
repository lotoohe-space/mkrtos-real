cmake_minimum_required(VERSION 3.13)
#-DINFONES
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 -Wa,-mimplicit-it=thumb ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 -Wa,-mimplicit-it=thumb ")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}  -DAT32F437ZMT7 -DUSE_STDPERIPH_DRIVER -DAT_SURF_F437_V1 -Wa,-mimplicit-it=thumb ")

file(
    GLOB deps
    ${CMAKE_CURRENT_SOURCE_DIR}/vnes/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vnes/*.S
    *.c
    *.S
)


add_executable(
    nes.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    nes.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
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
    nes.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/drivers/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/device_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/libraries/cmsis/cm4/core_support
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/project/at_surf_f437_board/
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/middlewares/i2c_application_library
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/ATSURFF437/nes_simulator

    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/vnes

    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_display
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_i2c
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pca9555

)
add_dependencies(
    nes.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    nes.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    nes_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S nes.elf > ${CMAKE_SOURCE_DIR}/build/output/nes.S
    COMMAND
    ${CMAKE_READELF} -a nes.elf > ${CMAKE_SOURCE_DIR}/build/output/nes.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S nes.elf nes.bin
    COMMAND
    ${CMAKE_SIZE} nes.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy nes.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/nes
    COMMAND
    cp nes.elf ${CMAKE_SOURCE_DIR}/build/output/nes.elf
    COMMAND
    cp ${CMAKE_CURRENT_SOURCE_DIR}/nes_rom/*.nes ${CMAKE_SOURCE_DIR}/build/output/cpio/
)

add_dependencies(nes_dump nes.elf)
 