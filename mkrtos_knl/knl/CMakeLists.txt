cmake_minimum_required(VERSION 3.13)


file(GLOB deps *.c *.S)

# if (NOT ${CONFIG_BUDDY_SLAB} STREQUAL "y")
    # list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/buddy.c)
    # list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/slab.c)
# endif()

if (NOT DEFINED CONFIG_BUDDY_SLAB)
    list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/buddy.c)
    list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/slab.c)
else()
    if (NOT CONFIG_BUDDY_SLAB STREQUAL "y")
        list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/buddy.c)
        list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/slab.c)
    endif()
endif()

if (NOT DEFINED CONFIG_ELF_LAUNCH)
    list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/elf64.c)
else()
    if (NOT CONFIG_ELF_LAUNCH STREQUAL "y")
        list(REMOVE_ITEM deps ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/elf64.c)
    endif()
endif()

file(GLOB mm_src mm/*.c mm/*.S)
list(APPEND deps ${mm_src})
if (DEFINED CONFIG_MMU)
    if (CONFIG_MMU STREQUAL "y")
        file(GLOB mm_mmu_src mm/mmu/*.c mm/mmu/*.S)
        list(APPEND deps ${mm_mmu_src})
    endif()
else()
file(GLOB mm_mpu_src mm/mpu/*.c mm/mpu/*.S)
list(APPEND deps ${mm_mpu_src})
endif()

add_library(knl STATIC ${deps})

target_include_directories(
    knl
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/${CONFIG_CPU_TYPE}
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/${CONFIG_ARCH}/

    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/lib
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/knl
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/arch/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/inc/drv

    ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/mm
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/mm/mmu
)
