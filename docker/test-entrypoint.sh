#!/bin/sh

set -e

PID_FILE="/tmp/app.pid"

build() {
    echo "==> Building..."
    cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build -j$(nproc)
    cp -r secrets/ build/
    echo "==> Build done"
}

start() {
    echo "==> Starting app..."
    gdb --batch -ex "handle SIGTERM pass" -ex run -ex bt -ex quit --args build/svc-auth &
    echo $! > "$PID_FILE"
    echo "==> PID $(cat $PID_FILE)"

    sleep 1
}

stop() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        echo "==> Stopping $PID"
        kill "$PID" 2>/dev/null || true
        wait "$PID" 2>/dev/null || true
        rm -f "$PID_FILE"
    fi
}

rebuild() {
    echo "==> Rebuild triggered"
    stop
    build
    start
}

trap rebuild USR1
trap "stop; exit 0" INT TERM


echo "==> Dev runner started"

build
start

while true; do
    sleep 0.5
done


