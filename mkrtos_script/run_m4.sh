#!/bin/bash

qemu-system-arm -machine\
 	netduinoplus2 -cpu cortex-m4 \
  	-nographic -m size=2 \
   	-kernel $PWD/build/output/kernel.img
