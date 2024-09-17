cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror" )

add_executable(uvmm.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(uvmm.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    --whole-archive
    ${START_LIB}
    libc_be
    --no-whole-archive
    printf
    sys
    sys_util
    sys_svr
    cjson
    ${GCC_LIB_PATH}/libgcc.a
)
# configure_file(src/benos.bin ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/uvmm.elf.dir/src/benos.bin COPYONLY)
# configure_file(src/vm_benos.json ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/uvmm.elf.dir/src/vm_benos.json COPYONLY)
target_include_directories(
    uvmm.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/uvmm/src

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cJSON

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/printf/src/printf
)
set_target_properties(uvmm.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker  "
)
add_custom_target(
    uvmm_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S uvmm.elf > ${CMAKE_SOURCE_DIR}/build/output/uvmm.S
    COMMAND
    ${CMAKE_READELF} -a uvmm.elf > ${CMAKE_SOURCE_DIR}/build/output/uvmm.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S uvmm.elf uvmm.bin
    COMMAND
    ${CMAKE_SIZE} uvmm.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp uvmm.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/uvmm
    COMMAND
    cp uvmm.elf ${CMAKE_SOURCE_DIR}/build/output/uvmm.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        uvmm_dump_elf ALL
        COMMAND
        cp uvmm.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/uvmm.elf
    )
    add_dependencies(uvmm_dump_elf uvmm_dump)
endif()

add_dependencies(uvmm_dump uvmm.elf)
add_dependencies(uvmm_dump sys)
add_dependencies(uvmm_dump sys_util)
add_dependencies(uvmm_dump sys_svr)
 