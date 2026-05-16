{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
nativeBuildInputs = with pkgs; [
    cmake
    ninja
    gdb
    clang-tools
    clang
  ];

  buildInputs = with pkgs; [
    postgresql
    jsoncpp
    libuuid
    zlib
    openssl
    libargon2
  ];

  CPATH = "${pkgs.glibc.dev}/include";
  LIBRARY_PATH = "${pkgs.glibc}/lib";
}
