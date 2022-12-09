cmake_minimum_required(VERSION 3.13)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} deps)
# file(GLOB_RECURSE deps ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.S)
if (BOARD STREQUAL "stm32f103rc")
    file(GLOB_RECURSE st032f103rc_cpu gd32f10x/*.c  gd32f10x/*.S)
    list(APPEND deps ${st032f103rc_cpu})
    message("${BOARD} devices deps: ${deps}")
elseif (BOARD STREQUAL "stm32f205rft6")
    file(GLOB_RECURSE stm32f205rft6 stm32f205rft6/*.c  stm32f205rft6/*.S)
    list(APPEND deps ${stm32f205rft6})
    # message("${BOARD} devices deps: ${deps}")
endif()

add_library(kernel_device OBJECT ${deps})
target_include_directories(
    kernel_device
    PUBLIC
    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
