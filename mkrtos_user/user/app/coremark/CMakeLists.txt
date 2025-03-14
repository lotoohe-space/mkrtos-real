cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast") 
# message("coremark==>"${CMAKE_C_FLAGS})
file(
    GLOB deps  
    coremark_src/*.c  
    *.c
)

add_executable(
    coremark.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    coremark.elf
    PUBLIC
    -Bstatic
    ${LIBC_NAME}
    ${START_LIB}
    sys
    sys_util
    sys_svr
    ${GCC_LIB_PATH}/libgcc.a
)
target_include_directories(
    coremark.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/coremark/coremark_src
    ${CMAKE_CURRENT_SOURCE_DIR}/coremark/
)
add_dependencies(
    coremark.elf
    ${START_LIB}
    sys
    sys_util
    # stm32f1_bsp
)
set_target_properties(
    coremark.elf PROPERTIES LINK_FLAGS
    "-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds ${CORTEX_M_LINK_FLAGS} --gc-section  -no-dynamic-linker "
    #--no-warn-rwx-segments
)
add_custom_target(
    coremark_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S coremark.elf > ${CMAKE_SOURCE_DIR}/build/output/coremark.S
    COMMAND
    ${CMAKE_READELF} -a coremark.elf > ${CMAKE_SOURCE_DIR}/build/output/coremark.txt
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S coremark.elf coremark.bin
    COMMAND
    ${CMAKE_SIZE} coremark.elf
    COMMAND
    ${CMAKE_COMMAND} -E copy coremark.bin ${CMAKE_SOURCE_DIR}/build/output/cpio/coremark
    COMMAND
    cp coremark.elf ${CMAKE_SOURCE_DIR}/build/output/coremark.elf
)

add_dependencies(coremark_dump coremark.elf)
 