cmake_minimum_required(VERSION 3.13)

# -n -pie -fpie -fpic 
# -msingle-pic-base -mno-pic-data-is-text-relative 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
 -D__dietlibc__ -D__arm__ -D__WORDSIZE=32 -D__ARM_ARCH_7M__ \
")
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})

include_directories(
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libc_backend/inc
)

add_subdirectory(init)
add_subdirectory(shell)
# add_subdirectory(app)
add_subdirectory(fs)
add_subdirectory(hello)
add_subdirectory(drv)
add_subdirectory(test)
add_subdirectory(net)
# add_subdirectory(tcc-0.9.27)
add_subdirectory(tinycc-arm-thumb)
