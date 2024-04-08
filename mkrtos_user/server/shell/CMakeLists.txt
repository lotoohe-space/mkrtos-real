cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(sh.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(sh.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    --whole-archive
    libc_be
    sys
    sys_util
    sys_svr
    # mr
    LetterShell
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    sh.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/letter-shell/demo/mkrtos
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/letter-shell/src
)
set_target_properties(sh.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
)
#--no-warn-rwx-segments
add_custom_target(
    shell_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S sh.elf > ${CMAKE_SOURCE_DIR}/build/output/sh.S
    COMMAND
    ${CMAKE_READELF} -a sh.elf > ${CMAKE_SOURCE_DIR}/build/output/sh.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S sh.elf sh.bin
    COMMAND
    ${CMAKE_SIZE} sh.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp sh.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/sh.elf
)

add_dependencies(shell_dump sh.elf)
add_dependencies(shell_dump sys)
add_dependencies(shell_dump sys_util)
# add_dependencies(shell_dump mr)
add_dependencies(shell_dump sys_svr)
# add_dependencies(shell_dump start)
# add_dependencies(shell_dump muslc)
add_dependencies(shell_dump LetterShell)
 