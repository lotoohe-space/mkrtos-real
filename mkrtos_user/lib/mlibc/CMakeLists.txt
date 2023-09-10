cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_XOPEN_SOURCE=700 ")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -D_XOPEN_SOURCE=700 ")

file(GLOB_RECURSE deps
src/*.C
src/*.c
src/*.S
src/*.s 
crt/*.c
crt/*.C
crt/*.S
crt/*.s)

list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/crt/start_init.S)
list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/crt/start.S)

add_library(start_init ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/crt/start_init.S)
add_library(start ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/crt/start.S)

add_library(muslc ${deps})
target_link_libraries(
    muslc
    PUBLIC
    libc_be
)
set_target_properties(muslc PROPERTIES LINK_FLAGS
"-pie "
)
target_include_directories(
    muslc
    BEFORE
    PUBLIC
    # ${CMAKE_SOURCE_DIR}/user/libc/backend
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)