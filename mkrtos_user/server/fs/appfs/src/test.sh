#!/bin/bash
set -xe
./build/appfs -g test -o out.img -s 33554432 -b 4096 -j
# ./build/appfs -t

