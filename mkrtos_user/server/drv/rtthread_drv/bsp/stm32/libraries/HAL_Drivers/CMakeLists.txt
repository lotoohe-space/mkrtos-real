cmake_minimum_required(VERSION 3.13)

add_subdirectory(drv_flash)

file(GLOB deps *.c *.S)


if(NOT DEFINED BSP_USING_ETH)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_eth.c)
endif()

if(NOT DEFINED RT_USING_HWCRYPTO)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_crypto.c)
endif()

if(NOT DEFINED RT_USING_PM OR NOT DEFINED SOC_SERIES_STM32L4)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_pm.c)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_lptim.c)
endif()

if(NOT DEFINED BSP_USING_PULSE_ENCODER)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_pulse_encoder.c)
endif()

if(NOT DEFINED RT_USING_I2C OR NOT DEFINED RT_USING_I2C_BITOPS )
if(NOT DEFINED BSP_USING_I2C1 
OR NOT DEFINED BSP_USING_I2C2 
OR NOT DEFINED BSP_USING_I2C3
OR NOT DEFINED BSP_USING_I2C4
)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_soft_i2c.c)
endif()
endif()

if(NOT DEFINED RT_USING_SPI)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_spi.c)
endif()

if(NOT DEFINED RT_USING_SPI_BITOPS)
list(REMOVE_ITEM deps ${CMAKE_CURRENT_LIST_DIR}/drv_soft_spi.c)
endif()

add_library(
    hal_drviers
    STATIC
    ${deps}
)
target_include_directories(
    hal_drviers
    PUBLIC

    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/config
    ${CMAKE_CURRENT_LIST_DIR}/drv_flash
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS/Include

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
    hal_drviers
    PUBLIC
    muslc
)
add_dependencies(hal_drviers muslc)


