cmake_minimum_required(VERSION 3.13)
# -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w \
-fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
-Wl,--gc-sections -D__arm__ -D__WORDSIZE=32 -D__ARM_ARCH_7M__ \
" )
set(CMAKE_ASM_FLAGS ${CMAKE_C_FLAGS})
# add_subdirectory(dietlibc)
add_subdirectory(sys)
add_subdirectory(sys_util)
add_subdirectory(sys_svr)
add_subdirectory(modbus)
add_subdirectory(stm32f1_bsp)
add_subdirectory(libc_backend)
add_subdirectory(mlibc)
add_subdirectory(cpio)
