cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    sharelib_test
    SHARED
    ${deps}
)
set_target_properties(sharelib_test PROPERTIES LINK_FLAGS " -pie ")

target_include_directories(
    sharelib_test
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
)


