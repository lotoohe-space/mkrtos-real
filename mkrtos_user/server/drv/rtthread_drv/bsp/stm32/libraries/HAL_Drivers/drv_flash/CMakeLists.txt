cmake_minimum_required(VERSION 3.13)


file(GLOB deps ${CMAKE_CURRENT_LIST_DIR}/drv_flash_f1.c)
add_library(
    drv_flash
    STATIC
    ${deps}
)
target_include_directories(
    drv_flash
    PUBLIC

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)
target_link_libraries(
    drv_flash
    PUBLIC
    muslc
)
add_dependencies(drv_flash muslc)


