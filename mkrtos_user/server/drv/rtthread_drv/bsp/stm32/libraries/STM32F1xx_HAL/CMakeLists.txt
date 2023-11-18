cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps  *.c *.S)

list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_timebase_rtc_alarm_template.c)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_timebase_tim_template.c)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_msp_template.c)

add_library(
    stm32f1xx_hal
    STATIC
    ${deps}
)
target_include_directories(
    stm32f1xx_hal
    PUBLIC

    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Device/ST/STM32F1xx
    ${CMAKE_CURRENT_LIST_DIR}/STM32F1xx_HAL_Driver/Inc

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
    stm32f1xx_hal
    PUBLIC
    muslc
)
add_dependencies(stm32f1xx_hal muslc)


