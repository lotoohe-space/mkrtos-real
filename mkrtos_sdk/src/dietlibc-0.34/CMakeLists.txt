cmake_minimum_required(VERSION 3.13)

add_subdirectory(arm)
add_subdirectory(lib)
add_subdirectory(libcompat)
add_subdirectory(libm)
add_subdirectory(libshell)
add_subdirectory(libstdio)
add_subdirectory(libugly)
add_subdirectory(profiling)
# add_subdirectory(softfpu)
add_subdirectory(syscalls.s)
add_subdirectory(builtins)
# set(SRC_C)
# set(REMOVE_SRC)

# file(GLOB_RECURSE deps **/*.c **/*.S *.c *.S)
# list(APPEND SRC_C ${deps})

# file(GLOB_RECURSE removes_syscall_s syscalls.s/*.S)
# list(APPEND REMOVE_SRC ${removes_syscall_s})

# message("REMOVE_SRC = ${REMOVE_SRC}")

# list(REMOVE_ITEM deps removes_syscall_s)

message("deps = ${deps}")

# add_library(dietlibc_lib OBJECT ${deps})
target_include_directories(
    dietlibc_lib
    PUBLIC
    include_directories(${CMAKE_SOURCE_DIR}/)
    include_directories(${CMAKE_SOURCE_DIR}/include)
    include_directories(${CMAKE_SOURCE_DIR}/softfpu)
)
