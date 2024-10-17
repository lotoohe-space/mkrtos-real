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
# add_custom_target(
#     bootstratp_link_file_dump ALL
#     COMMAND
#     ${CMAKE_C_COMPILER}
#     -include ${CMAKE_SOURCE_DIR}/build/autoconf.h
#     -E -P -<${CMAKE_CURRENT_LIST_DIR}/link.lds.S > 
#     ${CMAKE_CURRENT_LIST_DIR}/link.lds
# )

# add_dependencies(boot_bsp bootstratp_link_file_dump)