#!/bin/bash

export MYDIR=$PWD
export APP=mkrtos
export CMAKE_SIZE=$MYDIR/toolchains/gcc-arm-none-eabi-4_9-2014q4/bin/arm-none-eabi-size
export CMAKE_OBJCOPY=$MYDIR/toolchains/gcc-arm-none-eabi-4_9-2014q4/bin/arm-none-eabi-objcopy

set -e
sudo rm -f build/bin/*
cmake -G Ninja -DBOARD=stm32f205rft6 -B build/$APP .
cd build/$APP && ninja

cd ../..
# sudo mkdir build/bin
$CMAKE_SIZE build/mkrtos/mkrtos_bootstrap/mkrtos_bootstrap.elf
${CMAKE_OBJCOPY} -O binary build/mkrtos/mkrtos_bootstrap/mkrtos_bootstrap.elf build/mkrtos/mkrtos_bootstrap/mkrtos_bootstrap.bin
sudo cp build/mkrtos/mkrtos_bootstrap/mkrtos_bootstrap.bin build/bin/

export SHELL_NAME=mkrtos_shell
$CMAKE_SIZE build/mkrtos/$SHELL_NAME/$SHELL_NAME.elf
$CMAKE_OBJCOPY -O binary build/mkrtos/$SHELL_NAME/$SHELL_NAME.elf build/mkrtos/$SHELL_NAME/zh
sudo cp build/mkrtos/$SHELL_NAME/zh build/bin/

export LS_NAME=mkrtos_ls
$CMAKE_SIZE build/mkrtos/$LS_NAME/$LS_NAME.elf
$CMAKE_OBJCOPY -O binary build/mkrtos/$LS_NAME/$LS_NAME.elf build/mkrtos/$LS_NAME/ls
sudo cp build/mkrtos/$LS_NAME/ls build/bin/

export LS_NAME=mkrtos_ym
$CMAKE_SIZE build/mkrtos/$LS_NAME/$LS_NAME.elf
$CMAKE_OBJCOPY -O binary build/mkrtos/$LS_NAME/$LS_NAME.elf build/mkrtos/$LS_NAME/ym
sudo cp build/mkrtos/$LS_NAME/ym build/bin/

export TEST_NAME=mkrtos_test
$CMAKE_SIZE build/mkrtos/$TEST_NAME/$TEST_NAME.elf
$CMAKE_OBJCOPY -O binary build/mkrtos/$TEST_NAME/$TEST_NAME.elf build/mkrtos/$TEST_NAME/test
sudo cp build/mkrtos/$TEST_NAME/test build/bin/

$CMAKE_SIZE build/mkrtos/mkrtos_real/mkrtos_real.elf
$CMAKE_OBJCOPY -O binary build/mkrtos/mkrtos_real/mkrtos_real.elf build/mkrtos/mkrtos_real/kernel.bin
sudo cp build/mkrtos/mkrtos_real/kernel.bin build/bin

cd build/bin
# cpio的文件名长度必须要是n*4+2，文件的长度必须是4的整数倍
ls zh ls ym test | cpio -H newc -o > rootfs.cpio
srec_cat -output  kernel.img -binary \
        mkrtos_bootstrap.bin -binary\
        kernel.bin -binary -offset 0x00004000\
        rootfs.cpio -binary -offset 0x00040000

# sudo srec_cat -output  kernel.img -binary build/bin/mkrtos_bootstrap.bin -binary build/bin/kernel.bin -binary -offset 0x00004000 rootfs.cpio -binary -offset 0x00040000
#sudo srec_cat -output "mm.bin" -binary kernel.bin  -binary  mkrtos_shell.bin -binary -offset 0x00040000

