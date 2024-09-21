cmake_minimum_required(VERSION 3.13)

file(
    GLOB deps  
    *.c
)

add_executable(
    lxext4.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    lxext4.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    --whole-archive
    libc_be
    sys
    sys_util
    sys_svr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    lxext4.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/lxext4/ff15/source
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/lxext4

)
add_dependencies(
    lxext4.elf
    sys
    sys_util
)
set_target_properties(lxext4.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
)
add_custom_target(
    lxext4_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S lxext4.elf > ${CMAKE_SOURCE_DIR}/build/output/lxext4.S
    COMMAND
    ${CMAKE_READELF} -a lxext4.elf > ${CMAKE_SOURCE_DIR}/build/output/lxext4.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S lxext4.elf lxext4.bin
    COMMAND
    ${CMAKE_SIZE} lxext4.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp lxext4.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/lxext4
    COMMAND
    cp lxext4.elf ${CMAKE_SOURCE_DIR}/build/output/lxext4.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        lxext4_dump_elf ALL
        COMMAND
        cp lxext4.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/lxext4
    )
    add_dependencies(lxext4_dump_elf lxext4_dump)
endif()
add_dependencies(lxext4_dump lxext4.elf)
 