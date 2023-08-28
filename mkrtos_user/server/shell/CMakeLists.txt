cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(shell.elf
    ${deps}
)
target_link_libraries(shell.elf
    PUBLIC
    muslc
    sys
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    shell.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)
add_dependencies(shell.elf
muslc
)
set_target_properties(shell.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds -pie "
)
add_custom_target(
    shell_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S shell.elf > ${CMAKE_SOURCE_DIR}/build/output/shell.S
    COMMAND
    ${CMAKE_READELF} -a shell.elf > ${CMAKE_SOURCE_DIR}/build/output/shell.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S shell.elf shell.bin
    COMMAND
    ${CMAKE_SIZE} shell.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    cp shell.bin ${CMAKE_SOURCE_DIR}/build/output/shell
    COMMAND
    cp shell.elf ${CMAKE_SOURCE_DIR}/build/output/shell.elf
)

add_dependencies(shell_dump shell.elf)
 