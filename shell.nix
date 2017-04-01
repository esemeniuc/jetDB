with import <nixpkgs> {};
  stdenv.mkDerivation {
    name = "jetdb";
    buildInputs = [
      libpqxx cmake postgresql python35Packages.docker_compose
      python35 libsodium zeromq python35Packages.pyzmq
      SDL2 pkgconfig mesa
    ];
  }
