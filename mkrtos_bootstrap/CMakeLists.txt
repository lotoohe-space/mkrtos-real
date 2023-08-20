cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Werror \
-D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 \
-DSMP=1 \
-Wno-unused-parameter \
-Wno-unused-function \
-Wno-unused-variable \
")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/cortex-m3)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/stm32f205rft6)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/stm32f205rft6/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/dietlibc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/dietlibc/include)


add_subdirectory(src)

file(GLOB deps *.c *.S)

add_executable(bootstrap.elf
    ${deps}
)

set_target_properties(bootstrap.elf PROPERTIES LINK_FLAGS 
"-T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f205rft6_link.lds")

TARGET_LINK_LIBRARIES(bootstrap.elf
    boot_src
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
)
add_dependencies(bootstrap_dump bootstrap.elf)