#!/bin/bash
set -x
export PATH=$PATH:$TOOLCHAIN
SCRIPT_DIR=$(dirname "$0")
cd $SCRIPT_DIR
make distclean
./configure  --target=arm-none-eabi  --host=arm-none-eabi
cd bfd
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
cd ..
cd libiberty
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
cd ..
cd libctf
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
cd ..
cd zlib
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
cd intf
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
cd ..
cd ld
./configure  --target=arm-none-eabi  --host=arm-none-eabi
# cd ..
# cd binutils
# ./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
arm-none-eabi-objcopy -O binary -S ld-new ld-new.bin
cp ld-new.bin $PROJECT_PATH/build/output/cpio/
# cat config.log