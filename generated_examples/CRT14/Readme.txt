commands used:
nix run .#CRT12 -- $(./getScenePath.sh CRT14-Scene0)
make release && ./build/Release/crt_release $(./getScenePath.sh CRT14-Scene0)
nix run .#CRT12 -- $(./getScenePath.sh CRT14-Scene1)
make release && ./build/Release/crt_release $(./getScenePath.sh CRT14-Scene1)

CPU on the machine the tests were done: Ryzen 5 7600x

-------------------------------------------------------------
|                       |  CRT_12 branch  |  CRT_14 branch  |
=============================================================
|  scene 0 (triangles)  |        778ms    |       450ms     |
-------------------------------------------------------------
|    scene 1 (dragon)   |         295s    |         28s     |
-------------------------------------------------------------

