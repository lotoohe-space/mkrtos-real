cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_STDPERIPH_DRIVER=1 ")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-fPIC -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
 -D__dietlibc__ -D__arm__ -D__WORDSIZE=32 \
" )
set(CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS})


file(GLOB deps  
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/src/*.c 

)

add_library(
    mk_bsp
    STATIC
    ${deps}
)
target_include_directories(
    mk_bsp
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include
    ${CMAKE_SOURCE_DIR}/mkrtos_bsp/STM32/inc
)
# target_link_libraries(
#     mk_bsp
#     PUBLIC
#     muslc
# )
# add_dependencies(stm32f2_bsp muslc)


