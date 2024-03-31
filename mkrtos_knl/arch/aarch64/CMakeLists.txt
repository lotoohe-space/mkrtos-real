cmake_minimum_required(VERSION 3.13)

file(GLOB deps *.c *.S)
list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/aarch64/link.lds.S)
add_library(arch STATIC ${deps})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__MPU_PRESENT=1 -DUSE_STDPERIPH_DRIVER=1 ")
message(=======${CONFIG_CPU_TYPE})

if (${CONFIG_CPU_TYPE} STREQUAL "aarch64_qemu" )

    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${ARCH}/${CONFIG_CPU_TYPE}
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
        ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv
    )
    
    add_subdirectory(${CONFIG_CPU_TYPE})
endif()

target_link_libraries(
    arch
    PUBLIC
    knl_bsp
)
target_include_directories(
    arch
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl


)
