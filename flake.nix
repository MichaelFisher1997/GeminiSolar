{
  description = "Solar System Simulator using C++, SDL3, and Vulkan";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            cmake
            ninja
            clang
            pkg-config
            sdl3
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            vulkan-tools
            glm
            glew
            shaderc # For glslc to compile shaders
          ];

          shellHook = ''
            export VK_LAYER_PATH="${pkgs.vulkan-validation-layers}/share/vulkan/explicit_layer.d"
            echo "Solar System Simulator Dev Environment"
            echo "Compiler: $(clang++ --version | head -n1)"
          '';
        };

        packages.default = pkgs.stdenv.mkDerivation {
          pname = "space-sim";
          version = "0.1.0";
          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            ninja
            pkg-config
            shaderc
          ];

          buildInputs = with pkgs; [
            sdl3
            vulkan-headers
            vulkan-loader
            glm
            glew
          ];

          cmakeFlags = [
            "-DCMAKE_BUILD_TYPE=Release"
          ];
        };
      }
    );
}
