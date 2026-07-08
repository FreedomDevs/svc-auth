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
    libfido2.dev
    libfido2.out
    python3
  ];

  shellHook = ''
    export CPATH="${pkgs.glibc.dev}/include:$CPATH"
    export LIBRARY_PATH="${pkgs.glibc}/lib:$LIBRARY_PATH"
  '';
}
