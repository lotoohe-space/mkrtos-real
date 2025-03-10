cmake_minimum_required(VERSION 3.13)

file(
    GLOB deps
    *.c
)


add_executable(
    block.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    block.elf
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
    block.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_block
)
add_dependencies(
    block.elf
    ${START_LIB}
    sys
    sys_util
    mk_char
    mk_drv
    printf
    # mk_block
)
set_target_properties(
    block.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    block_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S block.elf > ${CMAKE_SOURCE_DIR}/build/output/block.S
    COMMAND
    ${CMAKE_READELF} -a block.elf > ${CMAKE_SOURCE_DIR}/build/output/block.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S block.elf block.bin
    COMMAND
    ${CMAKE_SIZE} block.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy block.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/block
    COMMAND
    cp block.elf ${CMAKE_SOURCE_DIR}/build/output/block.elf
)

add_dependencies(block_dump block.elf)
 