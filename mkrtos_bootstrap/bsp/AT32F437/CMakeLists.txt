cmake_minimum_required(VERSION 3.13)


file(GLOB_RECURSE deps 
    **/*.s
    *.s 
    **/*.C
    *.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/project/at_surf_f437_board/at_surf_f437_board_qspi_sram.c
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/AT32/AT32F435_437_Firmware_Library_V2.2.0/project/at_surf_f437_board/at_surf_f437_board_sdram.c
 
)

add_library(boot_bsp STATIC ${deps})
target_include_directories(
    boot_bsp
    PUBLIC
)
