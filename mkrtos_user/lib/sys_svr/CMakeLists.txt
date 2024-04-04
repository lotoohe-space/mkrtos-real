cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    sys_svr
    STATIC
    ${deps}
)
target_include_directories(
    sys_svr
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
)
target_link_libraries(
    sys_svr
    PUBLIC
    sys
    ${LIBC_NAME}
    cpio
    util
)
add_dependencies(sys_svr sys)
# add_dependencies(sys_svr muslc)


