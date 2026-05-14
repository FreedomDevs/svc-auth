{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    clang
    cmake
    ninja
    gdb

    postgresql
    jsoncpp
  ];
}
