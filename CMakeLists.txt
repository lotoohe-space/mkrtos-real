cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
SET(CMAKE_SYSTEM_NAME mkrtos) 
set(CMAKE_SYSTEM_PROCESSOR arm)

include(setting.cmake)
enable_language(ASM C CXX)
project(mkrtos)

execute_process(
    COMMAND date +%Y-%m-%d
    OUTPUT_VARIABLE DATE
)
execute_process(
    COMMAND date +%H:%M:%S
    OUTPUT_VARIABLE TIME
)
execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE COMMIT
)
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE BRANCH
)
string(STRIP ${DATE} DATE)
string(STRIP ${TIME} TIME)
set(code_version ${BRANCH}-${COMMIT} CACHE STRING " " FORCE)
set(compile_time "${DATE}\" \"${TIME}" CACHE STRING " " FORCE)


add_subdirectory(mkrtos_bootstrap)
add_subdirectory(mkrtos_knl)
# add_subdirectory(mkrtos_img)
# add_subdirectory(mkrtos_user)


