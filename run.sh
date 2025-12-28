#!/bin/sh
set -e

cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)

cd build
gdb --batch -ex run -ex bt -ex quit --args ./svc-auth
