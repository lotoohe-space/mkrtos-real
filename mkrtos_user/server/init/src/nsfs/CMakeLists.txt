cmake_minimum_required(VERSION 3.31.5)
project(nsfs VERSION 0.1.0 LANGUAGES C)
set(CMAKE_BUILD_TYPE Debug)
add_executable(nsfs ns.c nsfs.c main.c)
add_compile_options(
    "-Werror"
)

include(CTest)
enable_testing()

