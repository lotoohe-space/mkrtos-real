cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

list(REMOVE_ITEM deps 
"${CMAKE_CURRENT_SOURCE_DIR}/ipc/knl_msg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/ipc/knl_mutex.c"
"${CMAKE_CURRENT_SOURCE_DIR}/ipc/knl_sem.c"
)
add_library(kernel_knl STATIC ${deps})
target_include_directories(
    kernel_knl
    PUBLIC
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
