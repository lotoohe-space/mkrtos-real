cmake_minimum_required(VERSION 3.13)

add_subdirectory(coremark)
add_subdirectory(tinycc-arm-thumb)
add_subdirectory(test)
add_subdirectory(binutils-2.33.1)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_BOARD_NAME})
    add_subdirectory(${CONFIG_BOARD_NAME})
endif()
