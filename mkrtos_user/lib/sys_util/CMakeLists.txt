cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps  *.c *.S)
if (DEFINED CONFIG_ELF_LAUNCH)
    if (CONFIG_ELF_LAUNCH STREQUAL "y")
        list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/src/u_app_loader.c)
        list(APPEND deps ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/src/u_elf_loader.c)
    endif()
endif()

add_library(
    sys_util
    STATIC
    ${deps}
)
target_include_directories(
    sys_util
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
)
target_link_libraries(
    sys_util
    PUBLIC
    sys
    ${LIBC_NAME}
    cpio
    util
)
set_target_properties(sys_util PROPERTIES LINK_FLAGS ${CORTEX_M_LINK_FLAGS})
add_dependencies(sys_util sys)
# add_dependencies(sys_util ${LIBC_NAME})
add_dependencies(sys_util util)


