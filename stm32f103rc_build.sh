#!/bin/bash

export APP=mkrtos

set -e

cmake -G Ninja -B build/$APP .
cd build/$APP && ninja

