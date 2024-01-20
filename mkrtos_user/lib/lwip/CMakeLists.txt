cmake_minimum_required(VERSION 3.13)

file(GLOB
  deps

  ${CMAKE_CURRENT_LIST_DIR}/src/api/*.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/*.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/ipv4/*.c
  ${CMAKE_CURRENT_LIST_DIR}/src/netif/ethernet.c
  ${CMAKE_CURRENT_LIST_DIR}/contrib/ports/mkrtos/*.c

  ${CMAKE_CURRENT_LIST_DIR}/src/apps/lwiperf/lwiperf.c
)
message("lwip:${deps}")
add_library(
  lwip
  ${deps}
)

target_include_directories(
  lwip
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src/include
  ${CMAKE_CURRENT_LIST_DIR}/contrib/ports/mkrtos

  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include

  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/util/inc
  ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/sys/inc
)
target_link_libraries(
  lwip
  PUBLIC
  muslc
  sys
  util
)
add_dependencies(lwip
  muslc
  util
  sys
)
