cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps src/*.c src/*.S)

add_executable(net.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(net.elf
    PUBLIC
    ${START_LIB}
    ${LIBC_NAME}
    lwip
    --whole-archive
    libc_be
    sys
    sys_util
    sys_svr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    net.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/contrib/ports/mkrtos
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/lwip/src/include/lwip/apps
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/net/inc
)
set_target_properties(net.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker  "
)
add_custom_target(
    net_dump ALL
    COMMAND
    ${CMAKE_SIZE} net.elf
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S net.elf net.bin
    COMMAND
    ${CMAKE_COMMAND} -E copy net.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/net
    COMMAND
    ${CMAKE_COMMAND} -E copy net.elf ${CMAKE_SOURCE_DIR}/build/output/net.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        net_dump_elf ALL
        COMMAND
        ${CMAKE_COMMAND} -E copy net.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/net
    )
    add_dependencies(net_dump_elf net_dump)
endif()
add_dependencies(net_dump net.elf)
add_dependencies(net.elf sys)
add_dependencies(net.elf sys_util)
add_dependencies(net.elf sys_svr)
add_dependencies(net.elf lwip)
 