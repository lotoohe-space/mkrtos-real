cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_XL ")

file(
    GLOB deps  
)


add_executable(
    drv_fw.elf
    ${deps}
)
target_link_libraries(
    drv_fw.elf
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
    drv_fw.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/misc/vfs_lib/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/misc/fs_lib/inc
)
add_dependencies(
    drv_fw.elf
    start
    muslc
    sys
    sys_util
    stm32f1_bsp
)
set_target_properties(
    drv_fw.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    drv_fw_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S drv_fw.elf > ${CMAKE_SOURCE_DIR}/build/output/drv_fw.S
    COMMAND
    ${CMAKE_READELF} -a drv_fw.elf > ${CMAKE_SOURCE_DIR}/build/output/drv_fw.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S drv_fw.elf drv_fw.bin
    COMMAND
    ${CMAKE_SIZE} drv_fw.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp drv_fw.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/drv_fw
    COMMAND
    cp drv_fw.elf ${CMAKE_SOURCE_DIR}/build/output/drv_fw.elf
)

add_dependencies(drv_fw_dump drv_fw.elf)
 