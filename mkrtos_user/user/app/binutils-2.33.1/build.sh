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
make -j72
arm-none-eabi-objcopy -O binary -S ld-new ld-new.bin
cp ld-new.bin $PROJECT_PATH/build/output/cpio/ld


cd ..
cd opcodes
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72

cd ..
cd binutils
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
arm-none-eabi-objcopy -O binary -S objdump objdump.bin
cp objdump.bin $PROJECT_PATH/build/output/cpio/objdump
arm-none-eabi-objcopy -O binary -S objcopy objcopy.bin
cp objcopy.bin $PROJECT_PATH/build/output/cpio/objcopy
arm-none-eabi-objcopy -O binary -S readelf readelf.bin
cp readelf.bin $PROJECT_PATH/build/output/cpio/readelf
arm-none-eabi-objcopy -O binary -S size size.bin
cp size.bin $PROJECT_PATH/build/output/cpio/size
arm-none-eabi-objcopy -O binary -S nm-new nm-new.bin
cp nm-new.bin $PROJECT_PATH/build/output/cpio/nm

cd ..
cd gas
./configure  --target=arm-none-eabi  --host=arm-none-eabi
make -j72
arm-none-eabi-objcopy -O binary -S as-new as-new.bin
cp as-new.bin $PROJECT_PATH/build/output/cpio/as


# cat config.log