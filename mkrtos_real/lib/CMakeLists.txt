cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# list(REMOVE_ITEM deps 
# "${CMAKE_CURRENT_SOURCE_DIR}/__guard.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/__longjmp.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/clone.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/setjmp.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/strcpy.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/strlen.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/unified.S"
# "${CMAKE_CURRENT_SOURCE_DIR}/waitpid.S"
# )

add_library(kernel_lib STATIC ${deps})
# add_library(dietlibc_arm OBJECT ${SRC_FILES})
target_include_directories(
    kernel_lib
    PUBLIC
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
)
