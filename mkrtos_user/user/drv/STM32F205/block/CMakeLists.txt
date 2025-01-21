cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps  
    *.c 
    *.S
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/*.c
)
add_executable(
    block
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    block
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    mk_drv
    mk_char
    --whole-archive
    ${START_LIB}
    libc_be
    sys
    sys_util
    sys_svr
    --no-whole-archive
    ${GCC_LIB_PATH}/libgcc.a
)

target_include_directories(
    block
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/../mk_drv
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libfdt/lib/contrib

    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
)

set_target_properties(
    block PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    block_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S block > ${CMAKE_SOURCE_DIR}/build/output/block.S
    COMMAND
    ${CMAKE_READELF} -a block > ${CMAKE_SOURCE_DIR}/build/output/block.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S block block.bin
    COMMAND
    ${CMAKE_SIZE} block
    COMMAND
    ${CMAKE_COMMAND} -E copy block.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/block
    COMMAND
    cp block ${CMAKE_SOURCE_DIR}/build/output/block
)
add_dependencies(block_dump block)

add_dependencies(
    block
    fdt
    sys
    sys_util
    sys_svr
    mk_drv
)
