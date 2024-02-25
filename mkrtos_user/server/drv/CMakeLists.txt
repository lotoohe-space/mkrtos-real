cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMKRTOS_DRV ")

# add_subdirectory(mr_drv)
# add_subdirectory(rtthread_drv)


# add_subdirectory(dm9000_drv)
add_subdirectory(lcd_drv)