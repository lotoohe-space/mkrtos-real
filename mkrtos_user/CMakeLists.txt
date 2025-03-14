cmake_minimum_required(VERSION 3.13)


if (${CONFIG_ARCH} STREQUAL "cortex-m3" OR 
    ${CONFIG_ARCH} STREQUAL "cortex-m4" OR 
    ${CONFIG_ARCH} STREQUAL "cortex-m33"
    )
    # -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
    -fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
   \
    " )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
    -fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
  \
    " )
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} \
    -fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
    \
    " )
    #-Ttext-segment=0x0  -z max-page-size=0x4 -z common-page-size=0x4 
    set(CORTEX_M_LINK_FLAGS "-pie ")
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

include_directories(
    ${CMAKE_SOURCE_DIR}/mkrtos_user/lib/libc_backend/inc
)
if (${ARCH_NAME} STREQUAL "armv7_8m")
    set(START_LIB start)
    set(START_INIT_LIB start_init)

    # add_subdirectory(app)
elseif(${CONFIG_ARCH} STREQUAL "aarch64" )
    set(
        START_LIB
        ""
    )
    set(START_INIT_LIB "")

    set(
        START_SRC
        ${CMAKE_SOURCE_DIR}/build/libc/output/lib/crt1.o 
        ${CMAKE_SOURCE_DIR}/build/libc/output/lib/crti.o 
        ${CMAKE_SOURCE_DIR}/build/libc/output/lib/crtn.o
    )
    set(
        START_SRC_INIT
        ${CMAKE_SOURCE_DIR}/build/libc/output/lib/crt1_init.o 
        ${CMAKE_SOURCE_DIR}/build/libc/output/lib/crti.o 
        ${CMAKE_SOURCE_DIR}/build/libc/output/lib/crtn.o
    )

    # add_subdirectory(uvmm)
    # add_subdirectory(test)
endif()

add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(user)
