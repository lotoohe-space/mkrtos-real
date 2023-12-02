cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 \
")


add_executable(test.elf
    ${deps}
)
target_link_libraries(test.elf
    PUBLIC
    start
    muslc
    sys
    sys_util
    sys_svr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    test.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/test/bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/test/bsp/inc
)
add_dependencies(test.elf
muslc
)
set_target_properties(test.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section -no-dynamic-linker "
#--no-warn-rwx-segments
)
add_custom_target(
    test_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S test.elf > ${CMAKE_SOURCE_DIR}/build/output/test.S
    COMMAND
    ${CMAKE_READELF} -a test.elf > ${CMAKE_SOURCE_DIR}/build/output/test.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S test.elf test.bin
    COMMAND
    ${CMAKE_SIZE} test.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp test.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/test
    COMMAND
    cp test.elf ${CMAKE_SOURCE_DIR}/build/output/test.elf
)

add_dependencies(test_dump test.elf)
add_dependencies(test_dump sys)
add_dependencies(test_dump sys_util)
 