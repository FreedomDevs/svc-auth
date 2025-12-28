#!/bin/bash
set -e

cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Native
cmake --build build -j$(nproc)
