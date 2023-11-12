cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps *.c *.S)

add_library(mr-dev STATIC ${deps})
target_include_directories(
    mr-dev
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/dev
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

)

