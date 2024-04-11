cmake_minimum_required(VERSION 3.13)

file(GLOB deps src/*.c src/*.S)
file(GLOB arch_src src/${ARCH_NAME}/*.c src/${ARCH_NAME}/*.S)
list(APPEND deps ${arch_src})
add_library(
    libc_be
    STATIC
    ${deps}
)
set_target_properties(libc_be PROPERTIES LINK_FLAGS ${CORTEX_M_LINK_FLAGS})

if (${ARCH_NAME} STREQUAL "armv7_8m")
    target_include_directories(
        libc_be
        PUBLIC
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libc_be/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libc_backend/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    )
    target_link_libraries(
        libc_be
        PUBLIC
        muslc
        sys
        sys_svr
    )
elseif(${CONFIG_ARCH} STREQUAL "aarch64" )
    target_include_directories(
        libc_be
        PUBLIC
        # ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libc_backend/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mkrtos-musl/src/internal
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mkrtos-musl/arch/aarch64
    )
    target_link_libraries(
        libc_be
        PRIVATE
        ${LIBC_NAME}
        sys
        sys_svr
    )
endif()

add_dependencies(libc_be sys sys_svr)
