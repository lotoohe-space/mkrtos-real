#!/bin/bash

set -e -x

mkdir -p build
cmake -G Ninja -B build .
cd build && ninja
