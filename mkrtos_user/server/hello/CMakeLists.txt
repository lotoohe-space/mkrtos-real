cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(hello.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(hello.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    printf
    sys
    sys_util
    sys_svr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    hello.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/hello/src/test

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/hello/bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/hello/bsp/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/printf/src/printf
)
set_target_properties(sh.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
)
add_custom_target(
    hello_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S hello.elf > ${CMAKE_SOURCE_DIR}/build/output/hello.S
    COMMAND
    ${CMAKE_READELF} -a hello.elf > ${CMAKE_SOURCE_DIR}/build/output/hello.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S hello.elf hello.bin
    COMMAND
    ${CMAKE_SIZE} hello.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp hello.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/hello
    COMMAND
    cp hello.elf ${CMAKE_SOURCE_DIR}/build/output/hello.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        hello_dump_elf ALL
        COMMAND
        cp hello.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/hello.elf
    )
    add_dependencies(hello_dump_elf hello_dump)
endif()
add_dependencies(hello_dump hello.elf)
add_dependencies(hello_dump sys)
add_dependencies(hello_dump sys_util)
add_dependencies(hello_dump sys_svr)
 