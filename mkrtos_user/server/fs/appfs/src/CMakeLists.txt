cmake_minimum_required(VERSION 3.5.0)
project(appfs VERSION 0.1.0 LANGUAGES C)
set(CMAKE_BUILD_TYPE Debug)
add_executable(appfs appfs.c hw_block_test.c hw_block_sim.c main.c test.c appfs_open.c)
add_compile_options(
    "-Werror"
)

include(CTest)
enable_testing()

