#!/bin/sh
cd build
gdb --batch -ex run -ex bt -ex quit --args ./svc-auth
