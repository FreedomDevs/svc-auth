# --- Stage 1: Build ---
FROM alpine:3.24 AS builder

# Ставим инструменты для сборки
RUN apk add --no-cache clang cmake pkgconfig postgresql-dev samurai util-linux-dev jsoncpp-dev zlib-dev argon2-dev yaml-cpp-dev re2-dev build-base python3 libfido2-dev libfido2

WORKDIR /app
COPY build_aaguid.py build_aaguid.py
RUN mkdir include && python3 build_aaguid.py

COPY . .

# Сборка
RUN cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_C_COMPILER=/usr/bin/clang
RUN cmake --build build -j$(nproc)

RUN mkdir -p /app/dist/lib /app/dist/usr/lib && \
  cp /lib/ld-musl-x86_64.so.1 /app/dist/lib && \
  cp \
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
      /usr/lib/libfido2.so.1 \
      /usr/lib/libcbor.so.0.13 \
      /usr/lib/libudev.so.1 \
      /app/dist/usr/lib && \
  cp /app/build/svc-auth /app/build/regexes.yaml /app/dist

# --- Stage 2: Runtime ---
FROM scratch
COPY --from=builder /app/dist/ /
ENTRYPOINT ["/svc-auth"]

