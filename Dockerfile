# --- Stage 1: Build ---
FROM alpine:3.23.2 AS builder

# Ставим инструменты для сборки
RUN apk add --no-cache g++ cmake pkgconfig postgresql-dev samurai util-linux-dev jsoncpp-dev zlib-dev

WORKDIR /app
COPY . .

# Сборка
RUN cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc && cmake --build build -j$(nproc)

# --- Stage 2: Runtime ---
FROM alpine:3.23

# Только необходимые runtime-библиотеки
RUN apk add --no-cache libstdc++ libpq libuuid jsoncpp sqlite-libs

COPY --from=builder /app/build/svc-auth /svc-auth

EXPOSE 9007
CMD ["/svc-auth"]

