set(CMAKE_TOOLCHAIN_PATH $ENV{TOOLCHAIN} CACHE STRING " " FORCE)
set(CROSS_COMPILE ${CMAKE_TOOLCHAIN_PATH}arm-none-eabi- CACHE PATH "" FORCE)
set(GCC_LIB_PATH $ENV{TOOLCHAIN_LIB})

set(CMAKE_INSTALL_PATH "${CMAKE_BINARY_DIR}deploy" CACHE PATH "" FORCE)
set(CMAKE_C_COMPILER "${CROSS_COMPILE}gcc" CACHE PATH "" FORCE)
set(CMAKE_ASM_COMPILER "${CROSS_COMPILE}gcc" CACHE PATH "" FORCE)
set(CMAKE_CC_COMPILER "${CROSS_COMPILE}g++" CACHE PATH "" FORCE)
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++" CACHE PATH "" FORCE)
set(CMAKE_CPP_COMPILER "${CROSS_COMPILE}cpp" CACHE PATH "" FORCE)
set(CMAKE_LINKER "${CROSS_COMPILE}ld" CACHE PATH "" FORCE)
set(CMAKE_STRIP "${CROSS_COMPILE}strip" CACHE PATH "" FORCE)
set(CMAKE_OBJCOPY "${CROSS_COMPILE}objcopy" CACHE PATH "" FORCE)
set(CMAKE_OBJDUMP "${CROSS_COMPILE}objdump" CACHE PATH "" FORCE)
set(CMAKE_READELF "${CROSS_COMPILE}readelf" CACHE PATH "" FORCE)
set(CMAKE_SIZE "${CROSS_COMPILE}size" CACHE PATH "" FORCE)
set(CMAKE_NM "${CROSS_COMPILE}nm" CACHE PATH "" FORCE)
set(CMAKE_AR "${CROSS_COMPILE}ar" CACHE PATH "" FORCE)
set(CMAKE_SIZE "${CROSS_COMPILE}size" CACHE PATH "" FORCE)


# -mfloat-abi=soft  -u _printf_float 
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -Os -g -lc -lrdimon -mfloat-abi=soft \
-std=gnu11 -ffunction-sections -fdata-sections -fno-builtin \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker  \
--gc-sections -fno-stack-protector \
" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "-mcpu=cortex-m3 -mthumb -mno-thumb-interwork \
-mfix-cortex-m3-ldrd -O0 -g -std=c++11 \
-fmessage-length=0 -Xlinker --print-map -Wall -W -fno-stack-protector -g \ 
-mfloat-abi=soft -lc -lrdimon -u _printf_float \
-ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker -Wl,-gc-sections \
" CACHE STRING "" FORCE)

set(CMAKE_ASM_FLAGS "-mcpu=cortex-m3 -mthumb -O0 -g -mfloat-abi=soft -lc -lrdimon \
-u _printf_float -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker --gc-sections -fno-stack-protector \
" CACHE STRING "" FORCE)

set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group -o <TARGET>" CACHE INTERNAL " " FORCE)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group -o <TARGET>" CACHE INTERNAL " " FORCE)

set(ARCH "cortex-m3" CACHE STRING "" FORCE)

