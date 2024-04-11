message("========use armv7_8.cmake")

set(CMAKE_C_FLAGS "-mcpu=${CONFIG_ARCH} -O0 -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE}  -mthumb -mno-thumb-interwork -D=MKRTOS \
-std=gnu11 -ffunction-sections -fdata-sections -fno-builtin -u=_printf_float \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker  \
-fno-stack-protector -Wl,--gc-sections \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

set(CMAKE_CXX_FLAGS "-mcpu=${CONFIG_ARCH} -O0 -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE} -mthumb -mno-thumb-interwork -D=MKRTOS -std=c++11 \
-fmessage-length=0 -Xlinker --print-map -Wall -W -fno-stack-protector -g \ 
 -u=_printf_float \
-ffunction-sections -fdata-sections -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)

set(CMAKE_ASM_FLAGS "-mcpu=${CONFIG_ARCH} -O0 -g3 -mfloat-abi=${CONFIG_FLOAT_TYPE} -mthumb -mno-thumb-interwork -D=MKRTOS \
-u=_printf_float -std=gnu11 -ffunction-sections -fdata-sections -fno-builtin \
-nostartfiles -nodefaultlibs -nostdlib -nostdinc -Xlinker  -fno-stack-protector \
-include ${CMAKE_SOURCE_DIR}/build/autoconf.h \
" CACHE STRING "" FORCE)