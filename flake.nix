{
  description = "Your project description";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    nixpkgs-esp-dev.url = "github:mirrexagon/nixpkgs-esp-dev";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, nixpkgs-esp-dev, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };

        # specifying the exact target speeds up the initial tool build,
        # but more importantly, it limits the tools that get installed.
        # specifically, esp-idf ships an ancient version of clang-format
        # on linux, and this prevents that.
        devEnv = nixpkgs-esp-dev.packages.${system}.esp-idf-esp32s3;
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            devEnv
            pkgs.platformio
          ];
          shellHook = ''
            export PLATFORMIO_CORE_DIR=$PWD/.platformio
          '';
        };
      });
}
