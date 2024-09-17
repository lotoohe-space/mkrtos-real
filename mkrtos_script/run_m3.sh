#!/bin/bash
# netduino2
if [ -z "$1" ]; then
	echo "请输入板卡名字，使用默认板卡："
	echo "netduino2"
	set $1 "netduino2"
else  
	echo "$1" 
fi

qemu-system-arm -machine\
 	$1 -cpu cortex-m3 \
  	-nographic -m size=2 \
   	-kernel $PWD/build/output/kernel.img
