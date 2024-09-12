cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cutest/CuTest.c
)
add_library(
    cutest
    STATIC
    ${deps}
)
target_include_directories(
    cutest
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cutest/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc

)
target_link_libraries(
    cutest
    PUBLIC
    ${LIBC_NAME}
)
#add_dependencies(cutest muslc)


