__NO_TARGET:
	echo "Specify a target!"

debug: ./build/Debug/crt_debug
release: ./build/Release/crt_release

./build/Debug/crt_debug: src/main.cpp include/utils.hpp build/Debug
	g++ -std=c++23 -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion -I./include/ src/main.cpp -o ./build/Debug/crt_debug

./build/Release/crt_release: src/main.cpp include/utils.hpp build/Release
	g++ -std=c++23 -g -O3 -I./include/ -fno-omit-frame-pointer -Werror -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion src/main.cpp -o ./build/Release/crt_release

build/Debug:
	mkdir -p build/Debug

build/Release:
	mkdir -p build/Release

build:
	mkdir -p build

clean:
	rm -r build
