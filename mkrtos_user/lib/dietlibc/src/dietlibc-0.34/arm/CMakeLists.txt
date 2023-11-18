cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# aux_source_directory(./ SRC_FILES)
list(REMOVE_ITEM deps 
"${CMAKE_CURRENT_LIST_DIR}/__guard.S"
"${CMAKE_CURRENT_LIST_DIR}/__longjmp.S"
"${CMAKE_CURRENT_LIST_DIR}/clone.S"
"${CMAKE_CURRENT_LIST_DIR}/setjmp.S"
"${CMAKE_CURRENT_LIST_DIR}/strcpy.S"
"${CMAKE_CURRENT_LIST_DIR}/strlen.S"
"${CMAKE_CURRENT_LIST_DIR}/unified.S"
"${CMAKE_CURRENT_LIST_DIR}/waitpid.S"
)

# message("${GLOB_RECURSE}")
# message("deps = ${deps}")

add_library(dietlibc_arm OBJECT ${deps})
# add_library(dietlibc_arm OBJECT ${SRC_FILES})
target_include_directories(
    dietlibc_arm
    PUBLIC
    include_directories(${CMAKE_CURRENT_LIST_DIR}/)
)
