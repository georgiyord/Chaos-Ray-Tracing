__NO_TARGET:
	echo "Specify a target!"

crt_debug: src/main.cpp build/Debug
	g++ -std=c++20 -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer src/main.cpp -o ./build/Debug/crt_debug

crt_release: src/main.cpp build/Release
	g++ -std=c++20 -g -O3 src/main.cpp -fno-omit-frame-pointer -o ./build/Release/crt_release

build/Debug:
	mkdir -p build/Debug

build/Release:
	mkdir -p build/Release

build:
	mkdir -p build

clean:
	rm -r build
