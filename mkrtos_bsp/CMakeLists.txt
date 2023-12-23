cmake_minimum_required(VERSION 3.13)

include_directories(STM32)

if ($ENV{CPU_TYPE} STREQUAL "STM32")    
    add_subdirectory(STM32)
# elseif(${CPU_TYPE} STREQUAL "STM32F2x" )
#     include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/stm32f205rft6)
#     include_directories(${CMAKE_SOURCE_DIR}/mkrtos_bootstrap/inc/stm32f205rft6/lib)
#     add_subdirectory(bsp/STM32F2x)
endif()

