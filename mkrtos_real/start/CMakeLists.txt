cmake_minimum_required(VERSION 3.13)

if (BOARD STREQUAL "stm32f103rc")
    file(GLOB_RECURSE st032f103rc_cpu start_gd32f10x.S)
    list(APPEND deps ${st032f103rc_cpu})
    message("${BOARD} arch deps: ${deps}")
elseif (BOARD STREQUAL "stm32f205rft6")
    file(GLOB_RECURSE stm32f205rft6 startup_stm32f2xx.S isr_handler.S stm32f2xx.it.c sys_call_handler.S system_stm32f2xx.c init.S sche.S)
    list(APPEND deps ${stm32f205rft6})
    # message("${BOARD} arch deps: ${deps}")
endif()

add_library(kernel_start OBJECT ${deps})
target_include_directories(
    kernel_start
    PUBLIC
)
