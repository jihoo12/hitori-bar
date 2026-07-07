{
  description = "hitori-bar - a minimal Wayland status bar";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        packages = {
          default = pkgs.callPackage ./default.nix { };
          hitori-bar = pkgs.callPackage ./default.nix { };
        };
      }
    );
}
