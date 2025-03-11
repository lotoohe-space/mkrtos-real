cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ")
file(
    GLOB deps
    *.c
    source/*.c
    porting/*.c
)

add_executable(
    ym.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    ym.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    --whole-archive
    ${START_LIB}
    libc_be
    sys
    sys_util
    sys_svr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    ym.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/porting
)
add_dependencies(
    ym.elf
    ${START_LIB}
    sys
    sys_util
)
set_target_properties(
    ym.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    ym_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S ym.elf ym.bin
    COMMAND
    ${CMAKE_SIZE} ym.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy ym.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/ym
    COMMAND
    cp ym.elf ${CMAKE_SOURCE_DIR}/build/output/ym.elf
)

add_dependencies(ym_dump ym.elf)
 