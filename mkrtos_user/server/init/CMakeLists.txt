cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE deps *.c *.S)

add_executable(init.elf
    ${deps}
)
target_link_libraries(init.elf
    PUBLIC
    diet
)
set_target_properties(init.elf PROPERTIES LINK_FLAGS
"-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds")

add_custom_target(
    init_dump ALL
    COMMAND
    ${CMAKE_OBJDUMP} -s -S init.elf > ${CMAKE_SOURCE_DIR}/build/output/init.S
    COMMAND
    ${CMAKE_READELF} -a init.elf > ${CMAKE_SOURCE_DIR}/build/output/init.info
    COMMAND
    ${CMAKE_OBJCOPY} -O binary -S init.elf init.bin
    COMMAND
    mkdir -p ${CMAKE_SOURCE_DIR}/build/output
    COMMAND
    cp init.bin ${CMAKE_SOURCE_DIR}/build/output/init
    COMMAND
    cp init.elf ${CMAKE_SOURCE_DIR}/build/output/init.elf
)

add_dependencies(init_dump init.elf)
 