cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w \
-fPIC -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
 -D__dietlibc__ -D__arm__ -D__WORDSIZE=32 -D__ARM_ARCH_7M__ \
" )
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})

file(GLOB deps  *.c *.S)
add_library(
    mr
    STATIC
    ${deps}
)
target_include_directories(
    mr
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/dev
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mr
)
add_subdirectory(dev)
add_subdirectory(src)
add_subdirectory(bsp)

target_link_libraries(
    mr
    PUBLIC
    --whole-archive
    mr-dev
    mr-src
    mr-dd
    --no-whole-archive
    muslc
)
add_dependencies(mr muslc)
add_dependencies(mr mr-dev)
add_dependencies(mr mr-src)
add_dependencies(mr mr-dd)



