cmake_minimum_required(VERSION 3.13)
if (${CONFIG_ARCH} STREQUAL "cortex-m3" OR 
    ${CONFIG_ARCH} STREQUAL "cortex-m4" OR 
    ${CONFIG_ARCH} STREQUAL "cortex-m33"
    )
# -n -pie -fpie -fpic 
# -msingle-pic-base -mno-pic-data-is-text-relative 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-fPIC -fPIE -n -pie -fpie -fpic -msingle-pic-base -mno-pic-data-is-text-relative \
" )
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
# add_subdirectory(net)
# add_subdirectory(tcc-0.9.27)
add_subdirectory(tinycc-arm-thumb)
elseif(${CONFIG_ARCH} STREQUAL "aarch64" )
#TODO:
endif()
