cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    mk_drv
    STATIC
    ${deps}
)
target_link_libraries(
    mk_drv
    PRIVATE
    fdt
    sys
    sys_util
    sys_svr
)
add_dependencies(
    mk_drv
    fdt
    sys
    sys_util
    sys_svr
)
target_include_directories(
    mk_drv
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/user/drv/lib/mk_char
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libfdt/lib/contrib
)
