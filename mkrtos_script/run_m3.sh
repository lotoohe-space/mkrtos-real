#!/bin/bash

qemu-system-arm -machine\
 	netduino2 -cpu cortex-m3 \
  	-nographic -m size=2 \
   	-kernel $PWD/build/output/kernel.img
