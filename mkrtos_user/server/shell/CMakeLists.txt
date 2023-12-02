cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 \
")


add_executable(sh.elf
    ${deps}
)
target_link_libraries(sh.elf
    PUBLIC
    start
    muslc
    --whole-archive
    sys
    sys_util
    sys_svr
    mr
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

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/sh/bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/sh/bsp/inc
)
add_dependencies(sh.elf
muslc
)
set_target_properties(sh.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
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
    cp sh.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/sh
    COMMAND
    cp sh.elf ${CMAKE_SOURCE_DIR}/build/output/sh.elf
)

add_dependencies(shell_dump sh.elf)
add_dependencies(shell_dump sys)
add_dependencies(shell_dump sys_util)
add_dependencies(shell_dump mr)
add_dependencies(shell_dump sys_svr)
add_dependencies(shell_dump start)
add_dependencies(shell_dump muslc)
add_dependencies(shell_dump LetterShell)
 