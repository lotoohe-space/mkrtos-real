cmake_minimum_required(VERSION 3.13)


file(
    GLOB deps
    *.c
)

add_executable(
    drv_test.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    drv_test.elf
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
    drv_test.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_display
)
add_dependencies(
    drv_test.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    drv_test.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    drv_test_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S drv_test.elf > ${CMAKE_SOURCE_DIR}/build/output/drv_test.S
    COMMAND
    ${CMAKE_READELF} -a drv_test.elf > ${CMAKE_SOURCE_DIR}/build/output/drv_test.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S drv_test.elf drv_test.bin
    COMMAND
    ${CMAKE_SIZE} drv_test.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy drv_test.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/drv_test
    COMMAND
    cp drv_test.elf ${CMAKE_SOURCE_DIR}/build/output/drv_test.elf
)

add_dependencies(drv_test_dump drv_test.elf)
 