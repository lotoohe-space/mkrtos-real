cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    util
    STATIC
    ${deps}
)
target_include_directories(
    util
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc

)
target_link_libraries(
    util
    PUBLIC
    sys
    ${LIBC_NAME}
    cpio
)
set_target_properties(util PROPERTIES LINK_FLAGS "-pie ")
add_dependencies(util sys)
# add_dependencies(util muslc)


