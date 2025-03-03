cmake_minimum_required(VERSION 3.13)


file(
    GLOB deps
    *.c
)

add_executable(
    tst2.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    tst2.elf
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
    tst2.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_display
)
add_dependencies(
    tst2.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    tst2.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    tst2_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S tst2.elf tst2.bin
    COMMAND
    ${CMAKE_SIZE} tst2.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy tst2.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/tst2
    COMMAND
    cp tst2.elf ${CMAKE_SOURCE_DIR}/build/output/tst2.elf
)

add_dependencies(tst2_dump tst2.elf)
 