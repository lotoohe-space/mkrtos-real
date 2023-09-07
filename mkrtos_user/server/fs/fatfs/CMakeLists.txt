cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(
    fatfs.elf
    ${deps}
)
target_link_libraries(
    fatfs.elf
    PUBLIC
    muslc
    sys
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    fatfs.elf
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
add_dependencies(
    fatfs.elf
    muslc
)
set_target_properties(
    fatfs.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds -pie "
)
add_custom_target(
    fatfs_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S fatfs.elf > ${CMAKE_SOURCE_DIR}/build/output/fatfs.S
    COMMAND
    ${CMAKE_READELF} -a fatfs.elf > ${CMAKE_SOURCE_DIR}/build/output/fatfs.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S fatfs.elf fatfs.bin
    COMMAND
    ${CMAKE_SIZE} fatfs.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp fatfs.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/fatfs
    COMMAND
    cp fatfs.elf ${CMAKE_SOURCE_DIR}/build/output/fatfs.elf
)

add_dependencies(fatfs_dump fatfs.elf)
 