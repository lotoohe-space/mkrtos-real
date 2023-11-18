cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -w -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_XL \
-fPIC -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
 -D__dietlibc__ -D__arm__ -D__WORDSIZE=32 -D__ARM_ARCH_7M__ \
" )
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    stm32f1_bsp
    STATIC
    ${deps}
)
target_include_directories(
    stm32f1_bsp
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)
target_link_libraries(
    stm32f1_bsp
    PUBLIC
    sys
    muslc
    cpio
)
add_dependencies(stm32f1_bsp sys)
add_dependencies(stm32f1_bsp muslc)


