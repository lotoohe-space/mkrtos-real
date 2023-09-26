cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Werror \
-DUSE_STDPERIPH_DRIVER=1 \
-DSMP=1 \
-Wno-unused-parameter \
-Wno-unused-function \
-Wno-unused-variable \
-Wno-deprecated \
")

include_directories(${CMAKE_SOURCE_DIR}/)
include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/core/cortex-m3)
include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/lib)



file(GLOB deps *.c)

add_executable(bootstrap.elf
    ${deps}
)

set_target_properties(bootstrap.elf PROPERTIES LINK_FLAGS 
"-T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f205rft6_link.lds --gc-section ")

if (${BOARD_NAME} STREQUAL "STM32F1x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32F10X_XL ")
    
    include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/STM32F1x)
    include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/STM32F1x/lib)
    add_subdirectory(bsp/STM32F1x)
elseif(${BOARD_NAME} STREQUAL "STM32F2x" )
    include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/stm32f205rft6)
    include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/stm32f205rft6/lib)
    add_subdirectory(bsp/STM32F2x)
endif()

TARGET_LINK_LIBRARIES(bootstrap.elf
    bsp
)

add_custom_target(
    bootstrap_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S bootstrap.elf bootstrap.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    cp bootstrap.bin ${CMAKE_SOURCE_DIR}/build/output/bootstrap
    COMMAND
    cp bootstrap.elf ${CMAKE_SOURCE_DIR}/build/output/bootstrap.elf
    COMMAND
    ${CMAKE_SIZE} bootstrap.elf
)
add_dependencies(bootstrap_dump bootstrap.elf)