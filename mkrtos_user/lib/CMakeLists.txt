cmake_minimum_required(VERSION 3.13)

add_subdirectory(sys)

if (${CONFIG_ARCH} STREQUAL "cortex-m3" OR 
    ${CONFIG_ARCH} STREQUAL "cortex-m4" OR 
    ${CONFIG_ARCH} STREQUAL "cortex-m33"
    )
    # -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
    " )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
    " )
    # add_subdirectory(dietlibc)
    add_subdirectory(sys_util)
    add_subdirectory(sys_svr)
    add_subdirectory(modbus)
    add_subdirectory(stm32f1_bsp)
    add_subdirectory(libc_backend)
    add_subdirectory(mlibc)
    add_subdirectory(cpio)
    add_subdirectory(util)
    add_subdirectory(mr)
    add_subdirectory(lwip)
    add_subdirectory(printf)
    add_subdirectory(letter-shell/demo/mkrtos)
    # add_subdirectory(at_device)
elseif(${CONFIG_ARCH} STREQUAL "aarch64")
#TODO:
endif()