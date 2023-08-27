#!/bin/bash
###
 # @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 # @Date: 2023-08-14 09:47:54
 # @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 # @LastEditTime: 2023-08-14 11:56:33
 # @FilePath: /mkrtos-real/run.sh
 # @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
### 

# if [-z "$1"]; then
# 	echo "usage: ./debug.sh [tcp port]"
# 	exit
# fi

qemu-system-arm -machine\
 	netduino2 -cpu cortex-m3 \
  	-nographic -m size=2 \
   	-kernel $PWD/build/output/kernel.img
