cmake_minimum_required(VERSION 3.13)

# file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)

# # list(REMOVE_ITEM deps 
# # "${CMAKE_CURRENT_SOURCE_DIR}/test_app2.S"
# # )

# message("app:${deps}")
# add_library(kernel_app STATIC ${deps})
# target_include_directories(
#     kernel_app
#     PUBLIC
#     include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
# )
