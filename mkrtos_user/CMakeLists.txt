cmake_minimum_required(VERSION 3.13)


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

    set(CORTEX_M_LINK_FLAGS "-pie")
    set(LIBC_NAME "muslc")
    set(ARCH_NAME "armv7_8m")

    include_directories(
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/arm/
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/arch/generic
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/src/internal
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/include
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/src/internal
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/obj/include
        ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/mlibc/include
    )

elseif(${CONFIG_ARCH} STREQUAL "aarch64")
    set(CORTEX_M_LINK_FLAGS " --gc-section -z max-page-size=0x1000 -z common-page-size=0x1000 ")
    set(LIBC_NAME "c")
    set(ARCH_NAME "aarch64")

    include_directories(
        ${CMAKE_SOURCE_DIR}/build/libc/output/include
    )
endif()
add_subdirectory(server)
add_subdirectory(lib)
