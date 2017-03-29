with import <nixpkgs> {};
  stdenv.mkDerivation {
    name = "jetdb";
    buildInputs = [ libpqxx cmake postgresql python35Packages.docker_compose ];
  }
