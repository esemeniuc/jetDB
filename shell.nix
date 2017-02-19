with import <nixpkgs> {};
  stdenv.mkDerivation {
    name = "jetdb";
    buildInputs = [ libpqxx cmake postgresql ];
  }
