cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(test.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(test.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    --whole-archive
    libc_be
    --no-whole-archive
    printf
    sys
    sys_util
    sys_svr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    test.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/test/src/test

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/test/bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/test/bsp/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/printf/src/printf
)
set_target_properties(test.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
)
add_custom_target(
    test_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S test.elf > ${CMAKE_SOURCE_DIR}/build/output/test.S
    COMMAND
    ${CMAKE_READELF} -a test.elf > ${CMAKE_SOURCE_DIR}/build/output/test.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S test.elf test.bin
    COMMAND
    ${CMAKE_SIZE} test.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp test.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/test
    COMMAND
    cp test.elf ${CMAKE_SOURCE_DIR}/build/output/test.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        test_dump_elf ALL
        COMMAND
        cp test.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/test.elf
    )
    add_dependencies(test_dump_elf test_dump)
endif()
add_dependencies(test_dump test.elf)
add_dependencies(test_dump sys)
add_dependencies(test_dump sys_util)
add_dependencies(test_dump sys_svr)
 