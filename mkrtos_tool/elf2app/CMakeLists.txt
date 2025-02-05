cmake_minimum_required(VERSION 3.5.0)
project(elf2app VERSION 0.1.0 LANGUAGES C)

add_executable(elf2app main.c)

include(CTest)
enable_testing()

