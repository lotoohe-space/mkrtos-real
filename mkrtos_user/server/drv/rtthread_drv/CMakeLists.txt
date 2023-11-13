cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 -DSTM32F10X_XL ")

file(
    GLOB_RECURSE deps *.c *.C *.s *.S
)

add_executable(
    rtthread_drv.elf
    ${deps}
)
target_link_libraries(
    rtthread_drv.elf
    PUBLIC
    start
    muslc
    sys
    sys_util
    sys_svr
    stm32f1_bsp
    --whole-archive
    mr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    rtthread_drv.elf
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

    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/drivers/include
)
add_dependencies(
    rtthread_drv.elf
    start
    muslc
    sys
    sys_util
    stm32f1_bsp
    mr
)
set_target_properties(
    rtthread_drv.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds -pie --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    rtthread_drv_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S rtthread_drv.elf > ${CMAKE_SOURCE_DIR}/build/output/rtthread_drv.S
    COMMAND
    ${CMAKE_READELF} -a rtthread_drv.elf > ${CMAKE_SOURCE_DIR}/build/output/rtthread_drv.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S rtthread_drv.elf rtthread_drv.bin
    COMMAND
    ${CMAKE_SIZE} rtthread_drv.elf
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output/cpio
    COMMAND
    cp rtthread_drv.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/rtthread_drv
    COMMAND
    cp rtthread_drv.elf ${CMAKE_SOURCE_DIR}/build/output/rtthread_drv.elf
)

add_dependencies(rtthread_drv_dump rtthread_drv.elf)
 