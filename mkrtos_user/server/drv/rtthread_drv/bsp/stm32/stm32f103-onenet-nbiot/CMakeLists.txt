cmake_minimum_required(VERSION 3.13)

file(GLOB deps  *.c *.S 
${CMAKE_CURRENT_LIST_DIR}/board/CubeMX_Config/Src/stm32f1xx_hal_msp.c
)
add_library(
    stm32f103_onenet_nbiot
    STATIC
    ${deps}
)
target_include_directories(
    stm32f103_onenet_nbiot
    PUBLIC

    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/board/CubeMX_Config/Inc

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
    stm32f103_onenet_nbiot
    PUBLIC
    muslc
)
add_dependencies(stm32f103_onenet_nbiot muslc)


