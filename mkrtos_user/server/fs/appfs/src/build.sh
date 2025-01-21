#!/bin/bash

set -e -x
cmake -G Ninja -B build .
cd build && ninja
