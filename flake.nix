{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      mkCRT =
        N: hash:
        pkgs.stdenv.mkDerivation {
          pname = "CRT-Homework";
          version = N;

          src = pkgs.fetchFromGitHub {
            owner = "georgiyord";
            repo = "Chaos-Ray-Tracing";
            inherit hash;
            rev = if builtins.stringLength N == 1 then "CRT_0${N}" else "CRT_${N}";
          };

          buildPhase = ''
            make crt_release
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp build/Release/crt_release $out/bin/CRT-Homework
          '';
        };
    in
    {
      devShells.x86_64-linux.default = pkgs.mkShell {
        packages = with pkgs; [
          gdb # dedicated gdb with pretty printing
          clang-tools
          gimp # inspect .ppm files
          bear # create clangd compile_commands.json
          perf
          flamegraph
        ];

        inputsFrom = with pkgs; [ ];

        shellHook = "";
      };

      packages.x86_64-linux = {
        CRT02 = mkCRT "2" "sha256-HjoDE/W6PTA0Sj2ygWb3M3zPboKzvjSojHJ3kFg00cA=";
        CRT03 = mkCRT "3" "sha256-n9T/bjAXMRd8AaI49Te3VHOWDKOCfVE2Ga4EjDJDPRE=";
        CRT04 = mkCRT "4" "sha256-2D0F/vCz+XShO/GRsdHuu6fbwkr5l3tM3Kt4DMEZIS4=";
      };
    };
}
