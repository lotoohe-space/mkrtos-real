cmake_minimum_required(VERSION 3.13)

include(setting.cmake)
project(mkrtos_sdk)

set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} \
    -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
    -Wl,--gc-sections  -Wall  -g -D__dietlibc__ -D__arm__ -D__WORDSIZE=32\
     " )
set(CMAKE_CXX_FLAGS CMAKE_C_FLAGS)
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})

message("mkrtos-sdk CMAKE_CFLAGS:${CMAKE_C_FLAGS}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/arm/softfpu)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/softfpu)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/arm/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/include)

add_subdirectory(src)

add_library(${CMAKE_PROJECT_NAME}_sdk
    $<TARGET_OBJECTS:dietlibc_src>
    $<TARGET_OBJECTS:dietlibc_arm>
    $<TARGET_OBJECTS:dietlibc_lib>
    $<TARGET_OBJECTS:dietlibc_libcompat>
    $<TARGET_OBJECTS:dietlibc_libm>
    $<TARGET_OBJECTS:dietlibc_libshell>
    $<TARGET_OBJECTS:dietlibc_libstdio>
    $<TARGET_OBJECTS:dietlibc_libugly>
    $<TARGET_OBJECTS:dietlibc_profiling>
    # $<TARGET_OBJECTS:dietlibc_softfpu>
    $<TARGET_OBJECTS:dietlibc_builtins>
)

# set_target_properties(${CMAKE_PROJECT_NAME}_sdk PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/link.lds -pie")

