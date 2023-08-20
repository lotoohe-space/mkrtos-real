cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${GLOBAL_C_FLAG} -w \
    -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
    -Wl,--gc-sections  -Wall  -g -D__dietlibc__ -D__arm__ -D__WORDSIZE=32\
     " )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/arm/softfpu)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/softfpu)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/arm/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/include)

add_subdirectory(src)

add_library(diet
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

