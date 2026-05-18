{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
nativeBuildInputs = with pkgs; [
    cmake
    ninja
    gdb
    clang-tools
    clang
    pkg-config
  ];

  buildInputs = with pkgs; [
    postgresql
    yaml-cpp
    jsoncpp
    libuuid
    zlib
    openssl
    libargon2
    re2
  ];

  CPATH = "${pkgs.glibc.dev}/include";
  LIBRARY_PATH = "${pkgs.glibc}/lib";
}
