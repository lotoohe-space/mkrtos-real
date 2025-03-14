cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    fd
    STATIC
    ${deps}
)
target_include_directories(
    fd
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/fd/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
)


