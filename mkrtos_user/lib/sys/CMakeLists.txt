cmake_minimum_required(VERSION 3.13)


file(GLOB deps  src/*.c src/*.S)
file(GLOB deps_arch ${CONFIG_ARCH}/*.c ${CONFIG_ARCH}/*.S)
file(APPEND deps deps_arch)
add_library(
    sys
    STATIC
    ${deps}
)
target_include_directories(
    sys
    PUBLIC
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc

    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
)
# target_link_libraries(
#     sys
#     PUBLIC
#     muslc
# )
# add_dependencies(sys muslc)


