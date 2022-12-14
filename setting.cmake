set(CMAKE_TOOLCHAIN_PATH "${CMAKE_SOURCE_DIR}/toolchains/gcc-arm-none-eabi-4_9-2014q4/bin/" CACHE STRING " " FORCE)
set(CROSS_COMPILE ${CMAKE_TOOLCHAIN_PATH}arm-none-eabi- CACHE PATH "" FORCE)

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
set(CMAKE_NM "${CROSS_COMPILE}nm" CACHE PATH "" FORCE)
set(CMAKE_AR "${CROSS_COMPILE}ar" CACHE PATH "" FORCE)
set(CMAKE_SIZE "${CROSS_COMPILE}size" CACHE PATH "" FORCE)

SET(BOARD "stm32f103rc" CACHE STRING "select build type.")

if (BOARD STREQUAL "stm32f103rc")
    message("select stm32f103rc.")
elseif (BOARD STREQUAL "stm32f205rft6")
    message("select stm32f205rft6")
endif()

set(BUILD_RPATH "debug" CACHE STRING "debug/release" FORCE)
#debug
set(GLOBAL_C_FLAG "-mcpu=cortex-m3 -mthumb -O0 -g -mfloat-abi=soft -lc -lrdimon -u _printf_float -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker --gc-sections " CACHE STRING "" FORCE)
set(GLOBAL_CXX_FLAG "-mcpu=cortex-m3 -mthumb -O0 -g -mfloat-abi=soft -u _printf_float -std=gnu11  -ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker --gc-sections " CACHE STRING "" FORCE)
set(GLOBAL_ASM_FLAG "-mcpu=cortex-m3 -mthumb -O0 -g -mfloat-abi=soft -u _printf_float -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Wall -x assembler-with-cpp -E " CACHE STRING "" FORCE)

set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group -o <TARGET>" CACHE INTERNAL " " FORCE)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> --start-group <LINK_LIBRARIES> --end-group -o <TARGET>" CACHE INTERNAL " " FORCE)

# set(BOARD $ENV(BOARD) CACHE STRING "" FORCE)
set(ARCH "cortex-m3" CACHE STRING "" FORCE)
