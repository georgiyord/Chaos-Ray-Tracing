This repo contains the homeworks for Chaos Camp course's Raytracing section. Every homework has it's own branch and builds on top of previous ones.
Make is used for building. For building on Windows, use WSL or MSYS2. Alternatively, you can build manually with `cl.exe /std=c++23 ./src/CurrentHomeworks.cpp`. The process generates files in the current working directory, not where the executable lives.

Building:
`make crt_release`

Running:
`./build/Release/crt_release`

If a homework has requested that an image or other examples need to be generated, they will be stored under `/generated_examples/CRT_##`, where ## is the number of the assigned homework