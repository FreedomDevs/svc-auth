{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    clang
    cmake
    ninja
    gdb

    postgresql.dev
    jsoncpp
    libuuid
    zlib
    openssl
  ];

  CPATH = "${pkgs.glibc.dev}/include";
  LIBRARY_PATH = "${pkgs.glibc}/lib";
}
