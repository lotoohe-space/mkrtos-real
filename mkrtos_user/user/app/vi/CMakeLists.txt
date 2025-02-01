cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOPTPARSE_IMPLEMENTATION \
-DVI_ENABLE_VI_ASK_TERMINAL -DVI_ENABLE_SET -DVI_ENABLE_UNDO -DVI_ENABLE_8BIT \
-DVI_ENABLE_DOT_CMD ")
file(
    GLOB deps
    *.c
    vi-master/*.c
)

add_executable(
    vi.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    vi.elf
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
    vi.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_pin
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_display
)
add_dependencies(
    vi.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    vi.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    vi_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S vi.elf > ${CMAKE_SOURCE_DIR}/build/output/vi.S
    COMMAND
    ${CMAKE_READELF} -a vi.elf > ${CMAKE_SOURCE_DIR}/build/output/vi.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S vi.elf vi.bin
    COMMAND
    ${CMAKE_SIZE} vi.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy vi.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/vi
    COMMAND
    cp vi.elf ${CMAKE_SOURCE_DIR}/build/output/vi.elf
)

add_dependencies(vi_dump vi.elf)
 