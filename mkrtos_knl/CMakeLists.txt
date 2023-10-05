cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Werror \
-D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 \
-DKNL_OFFSET=$ENV{KEN_OFFSET} \
-DKNL_TEXT=$ENV{KNL_TEXT} \
-DINIT_OFFSET=$ENV{INIT_OFFSET} \
-DBOOTFS_ADDR_OFFSET=$ENV{BOOTFS_ADDR_OFFSET} \
-DSMP=1 \
-Wno-unused-parameter \
-Wno-unused-function \
-Wno-unused-variable \
")
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})


if (${BOARD_NAME} STREQUAL "STM32F1x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
elseif(${BOARD_NAME} STREQUAL "STM32F2x" )
endif()

message(编译参数：${CMAKE_C_FLAGS})
add_subdirectory(arch)
add_subdirectory(drivers)
add_subdirectory(knl)
add_subdirectory(lib)

add_executable(mkrtos.elf
   ${deps}
   dummy.c
)
target_link_libraries(
    mkrtos.elf
    PUBLIC
    knl
    arch
    drv
    lib
)
add_dependencies(mkrtos.elf arch)
add_dependencies(mkrtos.elf drv)
add_dependencies(mkrtos.elf knl)
add_dependencies(mkrtos.elf lib)

set_target_properties(mkrtos.elf PROPERTIES 
LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/stm32_link.lds -z max-page-size=0x1000 --gc-section ")

add_custom_target(
    link_file_dump ALL
    COMMAND
    ${CMAKE_C_COMPILER}
    -DKNL_TEXT=$ENV{KNL_TEXT}
    -DKNL_DATA=$ENV{KNL_DATA}
    -DKNL_OFFSET=$ENV{KEN_OFFSET}
    -DKNL_DATA_SIZE=$ENV{KNL_DATA_SIZE}
    -E -P -<${CMAKE_CURRENT_SOURCE_DIR}/stm32_link.lds.S> 
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32_link.lds
)

add_custom_target(
    mkrtos_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S mkrtos.elf mkrtos.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    cp mkrtos.bin ${CMAKE_SOURCE_DIR}/build/output/mkrtos
    COMMAND
    cp mkrtos.elf ${CMAKE_SOURCE_DIR}/build/output/mkrtos.elf
    COMMAND
    ${CMAKE_OBJDUMP} -s -S mkrtos.elf > ${CMAKE_SOURCE_DIR}/build/output/mkrtos.S
    COMMAND
    ${CMAKE_SIZE} mkrtos.elf
)
add_dependencies(mkrtos.elf link_file_dump)
add_dependencies(mkrtos_dump mkrtos.elf)