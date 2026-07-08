This repo contains the homeworks for Chaos Camp course's Raytracing section. Every homework has it's own branch and builds on top of previous ones.
Make is used for building. For building on Windows, use WSL or MSYS2. Alternatively, you can build manually with `cl.exe /std=c++20 ./src/main.cpp`. The process generates files in the current working directory, not where the executable lives.

Building:
`make crt_release`

Running:
`./build/Release/crt_release`
