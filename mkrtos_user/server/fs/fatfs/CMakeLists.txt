cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_XL ")

file(
    GLOB deps  
    ff15/source/*.c  
    # ext_disk_drv/*.c 
    ram_disk_drv/*.c
    *.c
)

# list(REMOVE_ITEM deps mkrtos_user/server/fs/fatfs/ram_disk_drv/*.*)

add_executable(
    fatfs.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    fatfs.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    sys
    sys_util
    sys_svr
    # stm32f1_bsp
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    fatfs.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/fatfs/ff15/source
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/fatfs

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/stm32f1_bsp/core_inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/fs/fatfs/ext_disk_drv
)
add_dependencies(
    fatfs.elf
    ${START_LIB}
    sys
    sys_util
    # stm32f1_bsp
)
set_target_properties(
    fatfs.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    fatfs_dump ALL
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
 