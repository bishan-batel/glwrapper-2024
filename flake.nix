{
  description = "A Nix-flake-based C/C++ development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    # nixgl.url = "github:nix-community/nixGL";
  };

  outputs = { self, nixpkgs }:
  let
    supportedSystems = [ "x86_64-linux" "aarch64-linux" ];
    forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
      pkgs = import nixpkgs { 
          inherit system; 
          # overlays = [ nixgl.overlay ]; 
      };
    });
  in {
    devShells = forEachSupportedSystem ({ pkgs }: {
      default = pkgs.mkShell.override {}
        {
          shellHook = /* bash */ ''
            export LD_LIBRARY_PATH=${pkgs.libxkbcommon}/lib:$LD_LIBRARY_PATH
          '';

          packages = with pkgs; [
              ninja
              cmake
              extra-cmake-modules
              cmakeCurses 
          ];

          buildInputs = with pkgs; [
            llvmPackages_19.lldb 
            llvmPackages_19.clang-tools
            llvmPackages_19.clang
            libffi
          ];

          nativeBuildInputs = with pkgs; [
            glfw-wayland
            wayland-protocols
            wayland
            libxkbcommon
              # llvm.lldb
              # boost
            clang-tools
          ];
        };
    });
  };
}
