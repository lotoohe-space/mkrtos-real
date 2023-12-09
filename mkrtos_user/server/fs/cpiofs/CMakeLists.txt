cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_XL ")

file(
    GLOB deps  
    *.c
)

# list(REMOVE_ITEM deps mkrtos_user/server/fs/cpiofs/ram_disk_drv/*.*)

add_executable(
    cpiofs.elf
    ${deps}
)
target_link_libraries(
    cpiofs.elf
    PUBLIC
    start
    muslc
    sys
    sys_util
    sys_svr
    stm32f1_bsp
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    cpiofs.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/cpio

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/cpiofs/ff15/source
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/cpiofs

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/cpiofs/ext_disk_drv
)
add_dependencies(
    cpiofs.elf
    start
    muslc
    sys
    sys_util
    stm32f1_bsp
)
set_target_properties(
    cpiofs.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    cpiofs_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S cpiofs.elf > ${CMAKE_SOURCE_DIR}/build/output/cpiofs.S
    COMMAND
    ${CMAKE_READELF} -a cpiofs.elf > ${CMAKE_SOURCE_DIR}/build/output/cpiofs.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S cpiofs.elf cpiofs.bin
    COMMAND
    ${CMAKE_SIZE} cpiofs.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp cpiofs.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/cpiofs
    COMMAND
    cp cpiofs.elf ${CMAKE_SOURCE_DIR}/build/output/cpiofs.elf
)

add_dependencies(cpiofs_dump cpiofs.elf)
 