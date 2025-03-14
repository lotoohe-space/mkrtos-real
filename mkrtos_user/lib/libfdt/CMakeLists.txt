cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps lib/contrib/*.c lib/contrib/*.S)
add_library(
    fdt
    STATIC
    ${deps}
)
target_include_directories(
    fdt
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libfdt/lib/contrib
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
)


