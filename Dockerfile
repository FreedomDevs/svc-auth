# --- Stage 1: Build ---
FROM alpine:3.23.2 AS builder

# Ставим инструменты для сборки
RUN apk add --no-cache g++ cmake pkgconfig postgresql-dev samurai util-linux-dev jsoncpp-dev zlib-dev argon2-dev

WORKDIR /app
COPY . .

# Сборка
RUN cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_C_COMPILER=/usr/bin/gcc && cmake --build build -j$(nproc)

# --- Stage 2: Runtime ---
FROM scratch

COPY --from=builder /lib/ld-musl-x86_64.so.1 /lib/

COPY --from=builder \
  /usr/lib/libssl.so.3 \
  /usr/lib/libcrypto.so.3 \
  /usr/lib/libjsoncpp.so.26 \
  /usr/lib/libuuid.so.1 \
  /usr/lib/libpq.so.5 \
  /usr/lib/libz.so.1 \
  /usr/lib/libstdc++.so.6 \
  /usr/lib/libgcc_s.so.1 \
  /usr/lib/libargon2.so.1 \
  /usr/lib/

COPY --from=builder /app/build/svc-auth /svc-auth

EXPOSE 9007
CMD ["/svc-auth"]

