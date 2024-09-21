cmake_minimum_required(VERSION 3.13)

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

    add_subdirectory(uvmm)
    # add_subdirectory(test)
endif()
add_subdirectory(init)
add_subdirectory(shell)
# add_subdirectory(tinycc-arm-thumb)
add_subdirectory(fs)
# add_subdirectory(drv)
# add_subdirectory(net)

