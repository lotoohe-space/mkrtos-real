cmake_minimum_required(VERSION 3.13)

file(
    GLOB deps
    *.c
)

add_executable(
    ram_block.elf
    ${deps}
    ${START_SRC}
)

target_link_libraries(
    ram_block.elf
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
    ram_block.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_block
)
add_dependencies(
    ram_block.elf
    ${START_LIB}
    sys
    sys_util
    mk_char
    mk_drv
    printf
)
set_target_properties(
    ram_block.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    ram_block_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S ram_block.elf ram_block.bin
    COMMAND
    ${CMAKE_SIZE} ram_block.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy ram_block.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/ram_block
    COMMAND
    cp ram_block.elf ${CMAKE_SOURCE_DIR}/build/output/ram_block.elf
)

add_dependencies(ram_block_dump ram_block.elf)
 