cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 \
")


add_executable(hello.elf
    ${deps}
)
target_link_libraries(hello.elf
    PUBLIC
    start
    muslc
    sys
    sys_util
    sys_svr
    mr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    hello.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/hello/src/test

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/hello/bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/hello/bsp/inc
)
add_dependencies(hello.elf
muslc
)
set_target_properties(hello.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
add_custom_target(
    hello_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S hello.elf > ${CMAKE_SOURCE_DIR}/build/output/hello.S
    COMMAND
    ${CMAKE_READELF} -a hello.elf > ${CMAKE_SOURCE_DIR}/build/output/hello.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S hello.elf hello.bin
    COMMAND
    ${CMAKE_SIZE} hello.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp hello.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/hello
    COMMAND
    cp hello.elf ${CMAKE_SOURCE_DIR}/build/output/hello.elf
)

add_dependencies(hello_dump hello.elf)
add_dependencies(hello_dump sys)
add_dependencies(hello_dump sys_util)
add_dependencies(hello_dump mr)
add_dependencies(hello_dump sys_svr)
add_dependencies(hello_dump start)
add_dependencies(hello_dump muslc)
 