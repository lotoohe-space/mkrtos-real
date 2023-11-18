cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# message("deps = ${deps}")

add_library(dietlibc_lib OBJECT ${deps})
# add_library(dietlibc_arm OBJECT ${SRC_FILES})

target_include_directories(
    dietlibc_arm
    PUBLIC

    include_directories(${CMAKE_CURRENT_LIST_DIR}/)
    # include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
    # include_directories(${CMAKE_CURRENT_LIST_DIR}/softfpu)
)
