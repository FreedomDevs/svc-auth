# --- Stage 1: Build ---
FROM alpine:3.23.4 AS builder

# Ставим инструменты для сборки
RUN apk add --no-cache clang cmake pkgconfig postgresql-dev samurai util-linux-dev jsoncpp-dev zlib-dev argon2-dev yaml-cpp-dev re2-dev

WORKDIR /app
COPY . .

# Сборка
RUN cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang && cmake --build build -j$(nproc)

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
  /usr/lib/libabsl_str_format_internal.so.2508.0.0 \
  /usr/lib/libabsl_strings.so.2508.0.0 \
  /usr/lib/libabsl_int128.so.2508.0.0 \
  /usr/lib/libabsl_strings_internal.so.2508.0.0 \
  /usr/lib/libabsl_raw_logging_internal.so.2508.0.0 \
  /usr/lib/


COPY --from=builder /app/build/svc-auth /app/build/regexes.yaml /

CMD ["/svc-auth"]

