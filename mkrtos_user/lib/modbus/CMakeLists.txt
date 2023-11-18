cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -w \
-fPIC -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
 -D__dietlibc__ -D__arm__ -D__WORDSIZE=32 -D__ARM_ARCH_7M__ \
" )
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})


file(GLOB_RECURSE deps  *.c *.S)
add_library(
    xtinymodbus
    STATIC
    ${deps}
)
target_include_directories(
    xtinymodbus
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/modbus_base
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/modbus_rtu_master
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/modbus_rtu_slave
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/port
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc/serial
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/modbus/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
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
    xtinymodbus
    PUBLIC
    sys
    sys_util
    muslc
)
add_dependencies(xtinymodbus sys)
add_dependencies(xtinymodbus muslc)


