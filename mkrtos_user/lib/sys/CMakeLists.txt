cmake_minimum_required(VERSION 3.13)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w \
-fPIC -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
-Wl,--gc-sections -D__dietlibc__ -D__arm__ -D__WORDSIZE=32 -D__ARM_ARCH_7M__ \
" )
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})


add_subdirectory(src)
file(GLOB deps  *.c *.S)
add_library(
    sys
    STATIC
    ${deps}
)
target_link_directories(
    sys
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dietlibc-0.34/include
)
target_link_libraries(
    sys
    PUBLIC
    diet
)
add_dependencies(sys diet)


