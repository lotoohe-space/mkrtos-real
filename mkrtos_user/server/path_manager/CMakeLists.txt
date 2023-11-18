cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(path_manager.elf
    ${deps}
)
target_link_libraries(path_manager.elf
    PUBLIC
    muslc
    sys
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    path_manager.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util
)
add_dependencies(path_manager.elf
muslc
)
set_target_properties(path_manager.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section "
)
add_custom_target(
    path_manager_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S path_manager.elf > ${CMAKE_SOURCE_DIR}/build/output/path_manager.S
    COMMAND
    ${CMAKE_READELF} -a path_manager.elf > ${CMAKE_SOURCE_DIR}/build/output/path_manager.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S path_manager.elf path_manager.bin
    COMMAND
    ${CMAKE_SIZE} path_manager.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp path_manager.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/path_manager
    COMMAND
    cp path_manager.elf ${CMAKE_SOURCE_DIR}/build/output/path_manager.elf
)

add_dependencies(path_manager_dump path_manager.elf)
 