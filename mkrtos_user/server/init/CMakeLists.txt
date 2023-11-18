cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(init.elf
    ${deps}
)
target_link_libraries(init.elf
    PUBLIC
    start_init
    muslc
    sys
    sys_util
    sys_svr
    cpio
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    init.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)
add_dependencies(init.elf
muslc
)
set_target_properties(init.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section  -no-dynamic-linker "
#--no-warn-rwx-segments
)
#-T ${CMAKE_CURRENT_LIST_DIR}/link.lds
add_custom_target(
    init_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S init.elf > ${CMAKE_SOURCE_DIR}/build/output/init.S
    COMMAND
    ${CMAKE_READELF} -a init.elf > ${CMAKE_SOURCE_DIR}/build/output/init.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S init.elf init.bin
    COMMAND
    ${CMAKE_SIZE} init.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    cp init.bin ${CMAKE_SOURCE_DIR}/build/output/init
    COMMAND
    cp init.elf ${CMAKE_SOURCE_DIR}/build/output/init.elf
)

add_dependencies(init_dump init.elf)
add_dependencies(init.elf muslc)
 