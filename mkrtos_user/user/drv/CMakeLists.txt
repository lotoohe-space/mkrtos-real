cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -DMKRTOS_DRV " )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -DMKRTOS_DRV ")
set(CMAKE_CXX_FLAGS "${CMAKE_ASM_FLAGS}  -DMKRTOS_DRV ")
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIG_BOARD_NAME})
    add_subdirectory(${CONFIG_BOARD_NAME})
endif()
add_subdirectory(lib)
add_subdirectory(common)
