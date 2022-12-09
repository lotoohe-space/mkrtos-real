cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps arm/cortext-m3/asm/*.S arm/cortex-m3/asm/*.c)
if (BOARD STREQUAL "stm32f103rc")
    file(GLOB_RECURSE st032f103rc_cpu cpu/gd32f103rc/*.c  cpu/gd32f103rc/*.S cpu/gd32f103rc/*.s)
    list(APPEND deps ${st032f103rc_cpu})
    message("${BOARD} arch deps: ${deps}")
elseif (BOARD STREQUAL "stm32f205rft6")
    file(GLOB_RECURSE stm32f205rft6 cpu/stm32f205rft6/*.c  cpu/stm32f205rft6/*.S cpu/stm32f205rft6/*.s)
    list(APPEND deps ${stm32f205rft6})
    # message("${BOARD} arch deps: ${deps}")
endif()

# message("flag:${CMAKE_C_FLAGS}")
add_library(kernel_arch STATIC ${deps})
target_include_directories(
    kernel_arch
    PUBLIC
    
    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
