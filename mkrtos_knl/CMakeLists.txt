cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-Werror \
-Wno-unused-parameter \
-Wno-unused-function \
-Wno-unused-variable \
-Wno-builtin-declaration-mismatch \
")

# -Ofast \
# message(编译参数：${CMAKE_C_FLAGS})

# cortex-m 系列处理器采用相同的内核arch
if(${CONFIG_ARCH} MATCHES "cortex-m")
    set(CONFIG_ARCH "cortex-m")
endif()

include_directories(
    ${CMAKE_SOURCE_DIR}/mkrtos_knl/knl/mm
)
add_subdirectory(arch)
add_subdirectory(knl)
add_subdirectory(lib)
add_subdirectory(test)

add_executable(mkrtos.elf
   ${deps}
   dummy.c
)

target_link_libraries(
    mkrtos.elf
    PUBLIC
    --whole-archive
    arch
    lib
    knl
    test
    --no-whole-archive
)
add_dependencies(mkrtos.elf arch)
add_dependencies(mkrtos.elf knl)
add_dependencies(mkrtos.elf lib)
add_dependencies(mkrtos.elf test)

set_target_properties(mkrtos.elf PROPERTIES 
LINK_FLAGS "-T ${CMAKE_CURRENT_LIST_DIR}/arch/${CONFIG_ARCH}/link.lds -z max-page-size=0x1000 --gc-section ")

add_custom_target(
    link_file_dump ALL
    COMMAND
    ${CMAKE_C_COMPILER}
    -include ${CMAKE_SOURCE_DIR}/build/autoconf.h
    -E -P -<${CMAKE_CURRENT_LIST_DIR}/arch/${CONFIG_ARCH}/link.lds.S> 
    ${CMAKE_CURRENT_LIST_DIR}/arch/${CONFIG_ARCH}/link.lds
)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/output)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/output/cpio)
add_custom_target(
    mkrtos_dump ALL
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S mkrtos.elf mkrtos.bin
    COMMAND
    ${CMAKE_COMMAND} -E copy  mkrtos.bin ${CMAKE_SOURCE_DIR}/build/output/mkrtos
    COMMAND
    ${CMAKE_COMMAND} -E copy  mkrtos.elf ${CMAKE_SOURCE_DIR}/build/output/mkrtos.elf
    COMMAND
    ${CMAKE_SIZE} mkrtos.elf
)
if ((DEFINED CONFIG_ELF_LAUNCH) AND (CONFIG_ELF_LAUNCH STREQUAL "y"))
    add_custom_target(
        mkrtos_dump_elf ALL
        COMMAND
        ${CMAKE_COMMAND} -E copy  mkrtos.elf ${CMAKE_SOURCE_DIR}/build/output/cpio/mkrtos.elf
    )
    add_dependencies(mkrtos_dump_elf mkrtos_dump)
endif()
add_dependencies(mkrtos.elf link_file_dump)
add_dependencies(mkrtos_dump mkrtos.elf)
add_dependencies(mkrtos_dump knl)
add_dependencies(mkrtos_dump arch)
add_dependencies(mkrtos_dump lib)


