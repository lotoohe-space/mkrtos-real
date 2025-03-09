cmake_minimum_required(VERSION 3.13)

file(
    GLOB deps  
    src/*.c
)

add_executable(
    procfs.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    procfs.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    sys
    sys_util
    sys_svr
    fd
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    procfs.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/procfs/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/procfs

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/fd
)
add_dependencies(
    procfs.elf
    ${START_LIB}
    sys
    sys_util
    fd
)
set_target_properties(
    procfs.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    procfs_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S procfs.elf procfs.bin
    COMMAND
    ${CMAKE_SIZE} procfs.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp procfs.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/procfs
    COMMAND
    cp procfs.elf ${CMAKE_SOURCE_DIR}/build/output/procfs.elf
)

add_dependencies(procfs_dump procfs.elf)
 