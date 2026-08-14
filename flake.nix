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

          passthru = {
            scene = "";
          };
        };
      unpackageScenesArchive =
        {
          name,
          package,
          scene,
          files,
        }:
        pkgs.stdenv.mkDerivation (finalAttributes: rec {
          src = package;
          inherit name;

          nativeBuildInputs = with pkgs; [
            p7zip
          ];

          unpackPhase = ''
            7z x $src
          '';

          copyInstructionsList = pkgs.lib.mapAttrsToList (
            fileOutName: fileInName: "cp ${fileInName} $out/${fileOutName}"
          ) files;

          mkDirInstructionsList = pkgs.lib.mapAttrsToList (
            fileOutName: fileInName: "mkdir -p $out/${dirOf fileOutName}"
          ) files;
          copyInstructions = pkgs.lib.concatStringsSep "\n" copyInstructionsList;
          mkDirInstructions = pkgs.lib.concatStringsSep "\n" mkDirInstructionsList;
          installPhase = ''
            mkdir -p $out;
            ${mkDirInstructions}
            ${copyInstructions}
          '';
          passthru = {
            inherit scene;
          };
        });
      materialsToTexturesFilter = ''
        .materials as $m
        | .textures += [
            $m | to_entries[]
            | select(.value.albedo | type == "array")
            | {name: ("albedo_" + (.key|tostring)), type: "albedo", albedo: .value.albedo}
          ]
        | .materials = [
            $m | to_entries[]
            | .value.albedo = (if (.value.albedo|type) == "array" then ("albedo_" + (.key|tostring)) else .value.albedo end)
            | .value
          ]
        | .objects |= map(if has("uvs") then . else .uvs = (.vertices | map(0)) end)
      '';
      patchScene =
        {
          package,
          jqFilter,
        }:
        pkgs.stdenv.mkDerivation {
          name = package.name + "-patched";
          src = package;
          dontUnpack = true;
          nativeBuildInputs = with pkgs; [
            jq
          ];

          installPhase = ''
            if [ -z "${package.passthru.scene}" ]; then
              cp "$src" "$out"
              srcFile="$out"
            else
              cp -r "$src/." "$out/"
              chmod -R u+w "$out"
              srcFile="$out/${package.passthru.scene}"
            fi
            jq '${jqFilter}' "$srcFile" > "$srcFile.tmp" \
              && mv "$srcFile.tmp" "$srcFile"
          '';

          passthru = {
            scene = package.passthru.scene;
          };
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

      packages.x86_64-linux = rec {
        CRT02 = mkCRT "2" "sha256-3aqgqKJcrNGlCT0kdeCuYO2iEmbKzNhVAQG0ziDg6Q0=";
        CRT03 = mkCRT "3" "sha256-6cewu4peRJyPAEpCzLajHf/TCdYe5LxdJXXHF6I3wR8=";
        CRT04 = mkCRT "4" "sha256-vuzJIkkHX3IPr58QH42eaZw+5i4OWs4mFC4QqC//THA=";
        CRT05 = mkCRT "5" "sha256-ZrZ6yhVqn9/VnwTZnkOH4/MhaHB20bABALlfSVIhUp4=";
        CRT06 = mkCRT "6" "sha256-6CWdbRIjWzNbPXeakfR6ADof31O9Lh56f42c9WGDN5w=";
        CRT07 = mkCRT "7" "sha256-i5kpOrmkMMmRCp8BUn6e6hFkqBArfYNFQRKYkXzT7To=";
        CRT08 = mkCRT "8" "sha256-jgFZw5hH3SwSYyTAIdgoypDUxMgp66Zrp/evv/k+sSY=";
        CRT09 = mkCRT "9" "sha256-IN6urR3SknrdRMIfDVouLsIQNxz1pvv4vlrn7ns+5hY=";
        # CRT10 and CRT11 had the same tasks
        CRT10 = mkCRT "10" "sha256-KrdWV6+mH5qz6Bax1KyX1zdk013hObAqnEvOjZGrUSE=";
        CRT11 = CRT10;
        CRT12 = mkCRT "12" "sha256-AHInC1yJ5TB3KMtN5g1rj5H90RH3xOF0k+4qiwXN6bc=";
        CRT14 = mkCRT "14" "";

        CRT07-Scene0 = fetchGoogleDrive {
          driveFileId = "12j-m2eP7bGAIVHLf71Y5avSc0gKTScro";
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

        CRT09-Task1-Scene0 = fetchGoogleDrive {
          driveFileId = "17-ggBUQYz_BDAGg9Fu0BgX3nIZZBPzn7";
          hash = "sha256-pQsStzxCpz2hSYvSxRngcSDjkylQcDw5HqAsjOqMjUM=";
        };
        CRT09-Task1-Scene1 = fetchGoogleDrive {
          driveFileId = "10qPP_07wj5n5qgdYVIm-JcekbkbeE_vy";
          hash = "sha256-9WyMqCkeqysZMYNJyPdnph3gSyR6s/NvBQgS6B66pVM=";
        };
        CRT09-Task2-Scene2 = fetchGoogleDrive {
          driveFileId = "14LYG5nj28YfLmE2ymuJpVPXRLZzy2msa";
          hash = "sha256-9WyMqCkeqysZMYNJyPdnph3gSyR6s/NvBQgS6B66pVM=";
        };
        CRT09-Task2-Scene3 = fetchGoogleDrive {
          driveFileId = "1yAc9Ei5wezpfbyfvTcq8DeBs7iudB0Ld";
          hash = "sha256-8QNl8r5/9lwB94rSFLG0SioiV5Rpovq93+U0aaXTWwg=";
        };
        CRT09-Task3-Scene4 = fetchGoogleDrive {
          driveFileId = "1eLmRS50u7QILHVnJTvCxuMEnCxcoFz_M";
          hash = "sha256-Vg7NyF4N4HDmyKdSGv1fPVoftbJikQzp4OrRo15Fx5o=";
        };
        CRT09-Task3-Scene5 = fetchGoogleDrive {
          driveFileId = "1dq8qgAjvuWTGTXNNJxbYqBqCZZXg0PjI";
          hash = "sha256-zqhJ9eItT3KRO04Xt2QqNHmDsnwBPLDU390RoOGeqCk=";
        };

        CRT10-Scene0 = fetchGoogleDrive {
          driveFileId = "1pY4bkjeBcdCcSA2rEXvVzAfPSRpIXg75";
          hash = "sha256-S9llSqqSjYQRbhtXX+Pb9ClejbSt3pdXHQwjqpJDXUI=";
        };
        CRT10-Scene1 = fetchGoogleDrive {
          driveFileId = "1hlEDmnUdjomEmIszVDdxcrntV4PGgJpa";
          hash = "sha256-YO1M9m4qncVyaXZ/feMBvsceMWZw7ZkxI70qgkYmA0c=";
        };
        CRT10-Scene2 = fetchGoogleDrive {
          driveFileId = "13fx_65lZUJlfLzKqCydy553FotGx72_1";
          hash = "sha256-Tb1h45BhLBiFghaG2X4NheH8olemmfdg41aZ3Xr1uJM=";
        };
        CRT10-Scene3 = fetchGoogleDrive {
          driveFileId = "17F17E1p1VS2OFptvNKEc9CGKpDQFfjIn";
          hash = "sha256-IsklBRU3N/mRVl3dQg1Fa7oUyPt51h2XrSjlAqB9aSY=";
        };
        CRT10-Scene4 = fetchGoogleDrive {
          driveFileId = "1kIZHhFE3gR2mO7PHdy-5BM8mTMWSpzFx";
          hash = "sha256-O4tSu5/1Zdiyz84pnDKJVuCdbVy4twEuBa+Bq9J8ah0=";
        };
        CRT10-Scene5 = fetchGoogleDrive {
          driveFileId = "1iiewSFty9rF7w4Dzz5wAiouKJ7rFP5NY";
          hash = "sha256-BgObMNL1vylNGrV0YqtVbrHiGCxnYNdgc99W07rGX3Y=";
        };
        CRT10-Scene6 = fetchGoogleDrive {
          driveFileId = "1regxdamU37OnHkofWPZABgFbPm5hnVqg";
          hash = "sha256-G9Z6VpYYkvu+F/MldUgfHA0WbCB14p/gRz8g7FvDwag=";
        };
        CRT10-Scene7 = fetchGoogleDrive {
          driveFileId = "19Ykb6ibzXLbLlWnAG41EClNC7R7ODKde";
          hash = "sha256-drMdreHAMf613+wMgPmfPlnos1Z4rEtdDc/1s+IrgCc=";
        };
        CRT10-Scene8 = fetchGoogleDrive {
          driveFileId = "1ntnrxOnKkaY1jPuW79hkZDjKJuXBIyt_";
          hash = "sha256-C4jxJGki1psqeT6nu6D4K10vDLwomt8yTroz1IpTMF4=";
        };

        # CRT10 and CRT11 had the same tasks
        CRT11-Scene0 = CRT10-Scene0;
        CRT11-Scene1 = CRT10-Scene1;
        CRT11-Scene2 = CRT10-Scene2;
        CRT11-Scene3 = CRT10-Scene3;
        CRT11-Scene4 = CRT10-Scene4;
        CRT11-Scene5 = CRT10-Scene5;
        CRT11-Scene6 = CRT10-Scene6;
        CRT11-Scene7 = CRT10-Scene7;
        CRT11-Scene8 = CRT10-Scene8;

        CRT12-Scenes = fetchGoogleDrive {
          driveFileId = "17UcE6Z13qWEIxl4oqJDax027xogNAoCm";
          hash = "sha256-bgme8QTzb8iMFsJIByBJJFiM4EepmLTtY+yXZoGzjx4=";
        };

        CRT12-Scene0 = unpackageScenesArchive {
          name = "CRT12-Scene0";
          package = CRT12-Scenes;
          scene = "scene.crtscene";
          files = {
            "scene.crtscene" = "scene0.crtscene";
            "textures/dragon.jpg" = "textures/dragon.jpg";
          };
        };
        CRT12-Scene1 = unpackageScenesArchive {
          name = "CRT12-Scene1";
          package = CRT12-Scenes;
          scene = "scene.crtscene";
          files = {
            "scene.crtscene" = "scene1.crtscene";
            "textures/dragon.jpg" = "textures/dragon.jpg";
          };
        };
        CRT12-Scene2 = unpackageScenesArchive {
          name = "CRT12-Scene2";
          package = CRT12-Scenes;
          scene = "scene.crtscene";
          files = {
            "scene.crtscene" = "scene2.crtscene";
            "textures/dragon.jpg" = "textures/dragon.jpg";
          };
        };
        CRT12-Scene3 = unpackageScenesArchive {
          name = "CRT12-Scene3";
          package = CRT12-Scenes;
          scene = "scene.crtscene";
          files = {
            "scene.crtscene" = "scene3.crtscene";
            "textures/dragon.jpg" = "textures/dragon.jpg";
          };
        };
        CRT12-Scene4 = unpackageScenesArchive {
          name = "CRT12-Scene4";
          package = CRT12-Scenes;
          scene = "scene.crtscene";
          files = {
            "scene.crtscene" = "scene4.crtscene";
            "textures/dragon.jpg" = "textures/dragon.jpg";
          };
        };
      };

      CRT13-Scene0 = patchScene {
        package = (
          fetchGoogleDrive {
            driveFileId = "1GtTwgo-wr-RdTTCnnDjn33sWaH7_iF-5";
            hash = "sha256-qRIIojMAfJhbHnG9AbG7rSaPeEUetdXejA2FHAqxcQc=";
          }
        );
        jqFilter = materialsToTexturesFilter;
      };

      CRT14-Scene0 = patchScene {
        package = (
          fetchGoogleDrive {
            driveFileId = "17AHbLsK7h6uK1WBptgr5Lmi2LF980xm-";
            hash = "sha256-vJJiUIfmKj2kst/1q2/gjbOhJwHi+5QQuvnA0aH/8wI=";
          }
        );
        jqFilter = materialsToTexturesFilter;
      };
      CRT14-Scene1 = patchScene {
        package = (
          fetchGoogleDrive {
            driveFileId = "1gqtpMqIYTC3K5kq4BdjarSnHSkDQ-AQs";
            hash = "sha256-qRIIojMAfJhbHnG9AbG7rSaPeEUetdXejA2FHAqxcQc=";
          }
        );
        jqFilter = materialsToTexturesFilter;
      };
    };
}
