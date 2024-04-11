cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    cpio
    STATIC
    ${deps}
)
target_include_directories(
    cpio
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
)


