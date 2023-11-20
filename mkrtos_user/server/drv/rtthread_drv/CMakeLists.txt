cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
 -DUSE_STDPERIPH_DRIVER=1\
 -DSTM32F10X_XL\
 -DUSE_HAL_DRIVER\
 -DSTM32F103xE\
 -DUSE_HAL_DRIVER\
 -DSTM32F103xE ")

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/components/drivers/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/stm32f103-onenet-nbiot/board
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/libraries/STM32F1xx_HAL/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/libraries/HAL_Drivers
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/libraries/STM32F1xx_HAL/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/stm32f103-onenet-nbiot/board/CubeMX_Config/Inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/components/drivers/hwcrypto
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/stm32f103-onenet-nbiot
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/components/drivers/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/server/drv/rtthread_drv/bsp/stm32/libraries/HAL_Drivers/CMSIS/Include
)

add_subdirectory(bsp/stm32)
add_subdirectory(components/drivers)
add_subdirectory(components/dfs)
add_subdirectory(src)

file(
    GLOB deps *.c *.C *.s *.S
)

add_executable(
    rtthread_drv.elf
    ${deps}
)
target_link_libraries(
    rtthread_drv.elf
    PUBLIC
    muslc
    --whole-archive
    start
    sys
    sys_util
    sys_svr
    src
    drivers
    stm32f103_onenet_nbiot
    stm32f1xx_hal
    hal_drviers
    drv_flash
    dfs
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


)
add_dependencies(
    rtthread_drv.elf
    start
    muslc
    sys
    sys_util
    src
    drivers
    stm32f103_onenet_nbiot
    stm32f1xx_hal
    hal_drviers
    drv_flash
    dfs
)
set_target_properties(
    rtthread_drv.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/link.lds -pie --gc-section  -no-dynamic-linker "
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
add_dependencies(rtthread_drv_dump muslc)
 