cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMKRTOS_DRV ")

# add_subdirectory(mr_drv)
# add_subdirectory(rtthread_drv)

add_subdirectory(${CONFIG_BOARD_NAME})

# if (${CONFIG_BOARD_NAME} STREQUAL "STM32F103ZET6")
#     add_subdirectory(dm9000_drv)
# elseif(${CONFIG_BOARD_NAME} STREQUAL "STM32F205" )

# elseif(${CONFIG_BOARD_NAME} STREQUAL "SWM34" )
#     add_subdirectory(lcd_drv)
# endif()
