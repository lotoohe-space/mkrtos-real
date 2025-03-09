message("========use armv7_8.cmake")

set(CMAKE_C_FLAGS "-mcpu=${CONFIG_ARCH} -Ofast -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE}  -mthumb  -DMKRTOS \
-std=gnu11 -ffunction-sections -fdata-sections -fno-builtin -u=_printf_float \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc \
-fno-stack-protector -Wl,--gc-section -D__ARM_ARCH_7M__ \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "-mcpu=${CONFIG_ARCH} -Ofast -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE} -mthumb -DMKRTOS -std=c++11 \
-fmessage-length=0 -Xlinker --print-map -Wall -W -fno-stack-protector \ 
 -u=_printf_float  -D__ARM_ARCH_7M__  \
-ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

set(CMAKE_ASM_FLAGS "-mcpu=${CONFIG_ARCH} -Ofast -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE} -mthumb -DMKRTOS \
-u=_printf_float -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker  -fno-stack-protector  -D__ARM_ARCH_7M__ \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

if (${CONFIG_FLOAT_TYPE} STREQUAL "hard")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMKRTOS_USE_FPU ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMKRTOS_USE_FPU ")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -DMKRTOS_USE_FPU ")
endif()

if (${CONFIG_FLOAT_TYPE} STREQUAL "softfp")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ")
endif()

