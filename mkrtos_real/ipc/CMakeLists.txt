cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

list(REMOVE_ITEM deps 
"${CMAKE_CURRENT_SOURCE_DIR}/knl_msg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/knl_mutex.c"
"${CMAKE_CURRENT_SOURCE_DIR}/knl_sem.c"
)

add_library(kernel_ipc STATIC ${deps})
# add_library(dietlibc_arm OBJECT ${SRC_FILES})
target_include_directories(
    kernel_ipc
    PUBLIC
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
