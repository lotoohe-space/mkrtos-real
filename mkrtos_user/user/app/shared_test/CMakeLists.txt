cmake_minimum_required(VERSION 3.13)
file(
    GLOB deps
    *.c
)

add_executable(
    shared_test.elf
    ${deps}
    ${START_SRC}
)
target_link_libraries(
    shared_test.elf
    PRIVATE
    c
)
target_include_directories(
    shared_test.elf
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys_svr/inc

    ${CMAKE_CURRENT_SOURCE_DIR}/
)
add_dependencies(
    shared_test.elf
    c
    sharelib_test
)
set_target_properties(
    shared_test.elf PROPERTIES 
    LINK_FLAGS "-pie -z max-page-size=0x0 -z common-page-size=0x0 --gc-sections -Ttext-segment=0x0 "
)
#-T ${CMAKE_CURRENT_LIST_DIR}/${ARCH_NAME}/link.lds 
set(CMAKE_C_LINK_EXECUTABLE
"${CMAKE_GOLD}  <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)
add_custom_target(
    shared_test_dump ALL
    COMMAND
    ${CMAKE_SIZE} shared_test.elf
    COMMAND
    cp shared_test.elf ${CMAKE_SOURCE_DIR}/build/output/shared_test.elf
)

add_dependencies(shared_test_dump shared_test.elf)
 