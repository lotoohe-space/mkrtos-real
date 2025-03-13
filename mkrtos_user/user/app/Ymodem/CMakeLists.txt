cmake_minimum_required(VERSION 3.13)

file(
    GLOB deps
    *.c
)

add_executable(
    rb.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    rb.elf
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
    rb.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/porting
)
add_dependencies(
    rb.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    rb.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    rb_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S rb.elf rb.bin
    COMMAND
    ${CMAKE_SIZE} rb.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy rb.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/rb
    COMMAND
    cp rb.elf ${CMAKE_SOURCE_DIR}/build/output/rb.elf
)

add_dependencies(rb_dump rb.elf)
 