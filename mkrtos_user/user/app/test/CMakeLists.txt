cmake_minimum_required(VERSION 3.13)


file(
    GLOB deps
    *.c
)

add_executable(
    tst.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    tst.elf
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
    tst.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_display
)
add_dependencies(
    tst.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    tst.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    tst_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S tst.elf > ${CMAKE_SOURCE_DIR}/build/output/tst.S
    COMMAND
    ${CMAKE_READELF} -a tst.elf > ${CMAKE_SOURCE_DIR}/build/output/tst.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S tst.elf tst.bin
    COMMAND
    ${CMAKE_SIZE} tst.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy tst.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/tst
    COMMAND
    cp tst.elf ${CMAKE_SOURCE_DIR}/build/output/tst.elf
)

add_dependencies(tst_dump tst.elf)
 