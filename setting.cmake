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
set(PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

set(BOARD $ENV{BOARD})
include(cmake/top.cmake)
set(ARCH ${CONFIG_ARCH} CACHE STRING "" FORCE)
message(=======${ARCH})
message(=======${CONFIG_CPU_TYPE})

# -mfloat-abi=soft  -u _printf_float   -lc -lrdimon
#-mfpu=fpv5-sp-d16
set(CMAKE_C_FLAGS "-mcpu=${CONFIG_ARCH} -O0 -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE}  -mthumb -mno-thumb-interwork -D=MKRTOS \
-std=gnu11 -ffunction-sections -fdata-sections -fno-builtin -u _printf_float \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker  \
-fno-stack-protector -Wl,--gc-sections \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "-mcpu=${CONFIG_ARCH} -O2 -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE} -mthumb -mno-thumb-interwork -D=MKRTOS -std=c++11 \
-fmessage-length=0 -Xlinker --print-map -Wall -W -fno-stack-protector -g \ 
 -u _printf_float \
-ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

set(CMAKE_ASM_FLAGS "-mcpu=${CONFIG_ARCH} -O2 -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE} -mthumb -mno-thumb-interwork -D=MKRTOS \
-u _printf_float -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker  -fno-stack-protector \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)



set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group -o <TARGET>" CACHE INTERNAL " " FORCE)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group -o <TARGET>" CACHE INTERNAL " " FORCE)



