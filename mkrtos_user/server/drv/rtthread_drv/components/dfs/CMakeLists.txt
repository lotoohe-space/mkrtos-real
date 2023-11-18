cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps  *.c *.S 
)
add_library(
    dfs
    STATIC
    ${deps}
)
target_include_directories(
    dfs
    PUBLIC

    ${CMAKE_CURRENT_LIST_DIR}/dfs_v1/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)
target_link_libraries(
    dfs
    PUBLIC
    muslc
)
add_dependencies(dfs muslc)


