cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps *.S *.s *.C *.c)

add_library(boot_bsp STATIC ${deps})
target_include_directories(
    boot_bsp
    PUBLIC
)

target_link_libraries(boot_bsp
    PRIVATE
)
