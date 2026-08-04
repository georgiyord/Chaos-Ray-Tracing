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

          makeTarget = if builtins.fromJSON N > 4 then "release" else "crt_release";

          buildPhase = ''
            make $makeTarget
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp build/Release/crt_release $out/bin/CRT-Homework
          '';
        };
      fetchGoogleDrive =
        { driveFileId, hash }:
        pkgs.fetchurl {
          url = "https://drive.google.com/uc?export=download&id=" + driveFileId;
          inherit hash;
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
          nix-direnv
          direnv
        ];

        inputsFrom = with pkgs; [ ];

        shellHook = ''
          make clean
          bear -- make debug
        '';
      };

      packages.x86_64-linux = {
        CRT02 = mkCRT "2" "sha256-HjoDE/W6PTA0Sj2ygWb3M3zPboKzvjSojHJ3kFg00cA=";
        CRT03 = mkCRT "3" "sha256-n9T/bjAXMRd8AaI49Te3VHOWDKOCfVE2Ga4EjDJDPRE=";
        CRT04 = mkCRT "4" "sha256-2D0F/vCz+XShO/GRsdHuu6fbwkr5l3tM3Kt4DMEZIS4=";
        CRT05 = mkCRT "5" "sha256-ZrZ6yhVqn9/VnwTZnkOH4/MhaHB20bABALlfSVIhUp4=";
        CRT06 = mkCRT "6" "sha256-6CWdbRIjWzNbPXeakfR6ADof31O9Lh56f42c9WGDN5w=";
        CRT07 = mkCRT "7" "sha256-i5kpOrmkMMmRCp8BUn6e6hFkqBArfYNFQRKYkXzT7To=";
        CRT08 = mkCRT "8" "sha256-jgFZw5hH3SwSYyTAIdgoypDUxMgp66Zrp/evv/k+sSY=";

        CRT07-Scene0 = fetchGoogleDrive {
          driveFileId = "11j-m2eP7bGAIVHLf71Y5avSc0gKTScro";
          hash = "sha256-RIa3ib28Q7kvm+hzSnk89VPGYPlsI58qfQwZVMHukQM=";
        };
        CRT07-Scene1 = fetchGoogleDrive {
          driveFileId = "10LTURJvKSUtC2gjPU0EHG-okI9xz1bER";
          hash = "sha256-6pzWFJUwUQYy4pAWcoZRi0R/fxiNgzbFqM8sugUaEEw=";
        };
        CRT07-Scene2 = fetchGoogleDrive {
          driveFileId = "1WcwPWip6JvqXSy96wnr5nWoGUlMHFyMi";
          hash = "sha256-ispiKtvbvtHNNaiVhl+XmC4zJCr8DgeCsXgzPI1iO38=";
        };
        CRT07-Scene3 = fetchGoogleDrive {
          driveFileId = "1a_j-zkRdGL8ZVtLGsQ8MIP1evhka7WpG";
          hash = "sha256-qlJNEi9k2ikL2SUvIQrNfclDOob3Zilq5++kj+pkRn0=";
        };
        CRT07-Scene4 = fetchGoogleDrive {
          driveFileId = "10xKWk2CkCDMWzuCD8dhHQg1c9UmM8DnY";
          hash = "sha256-Vmdyx0iW+t3w1Nu8v6wzRqZisWvioKLW1MJWKSDvqtg=";
        };

        CRT08-Scene0 = fetchGoogleDrive {
          driveFileId = "14F0LY6KnYAu7YeIKh5bGuu8k6Pt2bdmo";
          hash = "sha256-hgykRFzhkEAk5tXGnsqvdmnSf/5RkLaDvb1nkmfuU3k=";
        };
        CRT08-Scene1 = fetchGoogleDrive {
          driveFileId = "1-tXJm07NzMMyCFSq8PuyDPDw2Uj9yCWv";
          hash = "sha256-2qEhcXmkymWc6FLlL+V4qU+O2/9qKT9Fg3Umq8nl1b4=";
        };
        CRT08-Scene2 = fetchGoogleDrive {
          driveFileId = "1jnfGUgSW-NZE4iQXtkNQSj8EnVisQuF6";
          hash = "sha256-kKm8MtrCROAtfzcp6zrIBk7PkhKXt+E7RcNXLRzF2V8=";
        };
        CRT08-Scene3 = fetchGoogleDrive {
          driveFileId = "10WpZDl70ZdENGGxrythDj55QAkHuFZMK";
          hash = "sha256-vBkpu4I/NJAQ1SGzFWxhbxYo67b1rjFj7ng/eCK6wHc=";
        };
      };
    };
}
