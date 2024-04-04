cmake_minimum_required(VERSION 3.13)


file(GLOB deps  src/*.c src/*.S)
file(GLOB deps_arch src/${ARCH_NAME}/*.c src/${ARCH_NAME}/*.S)
list(APPEND deps ${deps_arch})
# message(======sys_list:${deps})
add_library(
    sys
    STATIC
    ${deps}
)
target_include_directories(
    sys
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc/${ARCH_NAME}

)
