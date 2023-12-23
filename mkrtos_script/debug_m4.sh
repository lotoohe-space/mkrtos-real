#!/bin/bash

# if [-z "$1"]; then
# 	echo "usage: ./debug.sh [tcp port]"
# 	exit
# fi

qemu-system-arm -machine\
 	netduinoplus2 -cpu cortex-m4 \
  	-nographic -m size=2\
   	-kernel $PWD/build/output/kernel.img \
    -S -gdb tcp::$1
