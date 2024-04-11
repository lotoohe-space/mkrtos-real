cmake_minimum_required(VERSION 3.13)

file(
    GLOB deps  
    *.c
)

# list(REMOVE_ITEM deps mkrtos_user/server/fs/cpiofs/ram_disk_drv/*.*)
message(==========="START_SRC:${START_SRC}")
add_executable(
    cpiofs.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    cpiofs.elf
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
    cpiofs.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/cpiofs/ff15/source
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/cpiofs

)
add_dependencies(
    cpiofs.elf
    sys
    sys_util
)
set_target_properties(cpiofs.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
)
add_custom_target(
    cpiofs_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S cpiofs.elf > ${CMAKE_SOURCE_DIR}/build/output/cpiofs.S
    COMMAND
    ${CMAKE_READELF} -a cpiofs.elf > ${CMAKE_SOURCE_DIR}/build/output/cpiofs.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S cpiofs.elf cpiofs.bin
    COMMAND
    ${CMAKE_SIZE} cpiofs.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp cpiofs.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/cpiofs
    COMMAND
    cp cpiofs.elf ${CMAKE_SOURCE_DIR}/build/output/cpiofs.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        cpiofs_dump_elf ALL
        COMMAND
        cp cpiofs.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/cpiofs
    )
    add_dependencies(cpiofs_dump_elf cpiofs_dump)
endif()
add_dependencies(cpiofs_dump cpiofs.elf)
 