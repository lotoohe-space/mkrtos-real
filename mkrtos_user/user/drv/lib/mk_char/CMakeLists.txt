cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    mk_char
    STATIC
    ${deps}
)
add_dependencies(
    mk_char
    fdt
    sys
    sys_util
    sys_svr
)
target_include_directories(
    mk_char
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/../mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libfdt/lib/contrib
)
