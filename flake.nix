{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in
    {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          clang-tools
          gimp # inspect .ppm files
          bear # create clangd compile_commands.json
          perf
          flamegraph
        ];

        inputsFrom = with pkgs; [ ];

        shellHook = "";
      };
    };
}
