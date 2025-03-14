cmake_minimum_required(VERSION 3.13)

file(GLOB deps src/*.c src/*.S src/test/*.c src/nsfs/ns.c src/nsfs/nsfs.c)
file(GLOB arch_src src/test/${ARCH_NAME}/*.c src/test/${ARCH_NAME}/*.S)

if (NOT $ENV{MKRTOS_TEST_MODE} STREQUAL "normal")
    message(STATUS "MKRTOS_TEST_MODE is $ENV{MKRTOS_TEST_MODE}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMKRTOS_TEST_MODE" )
endif()

add_executable(init.elf
    ${deps}
    ${arch_src}
    ${START_SRC_INIT}
)
target_link_libraries(init.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_INIT_LIB}
    --whole-archive
    libc_be
    sys
    sys_util
    sys_svr
    cpio
    util
    cutest
    --no-whole-archive
    appfs_tiny
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    init.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cutest

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/init/src
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/init/src/nsfs
)
set_target_properties(init.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
#--no-warn-rwx-segments
)
#-T ${CMAKE_CURRENT_LIST_DIR}/link.lds
add_custom_target(
    init_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S init.elf > ${CMAKE_SOURCE_DIR}/build/output/init.S
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S init.elf init.bin
    COMMAND
    ${CMAKE_SIZE} init.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy init.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/init
    COMMAND
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/board/${CONFIG_BOARD_NAME}/init.cfg ${CMAKE_SOURCE_DIR}/build/output/cpio/
    COMMAND
    ${CMAKE_COMMAND} -E copy init.elf ${CMAKE_SOURCE_DIR}/build/output/init.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        init_dump_elf ALL
        COMMAND
        cp init.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/init
    )
    add_dependencies(init_dump_elf init_dump)
endif()
add_dependencies(init_dump init.elf)
add_dependencies(init.elf libc_be util appfs_tiny)
