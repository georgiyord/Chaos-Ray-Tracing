__NO_TARGET:
	echo "Specify a target!"

DEFINITIONS ?= 

CXX      = g++
CXXFLAGS = -std=c++26 -I./include/ -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion $(DEFINITIONS)

CXXFLAGS_DEBUG = $(CXXFLAGS) -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer
CXXFLAGS_RELEASE = $(CXXFLAGS) -g -O3 -Werror

debug: ./build/Debug/crt_debug
release: ./build/Release/crt_release

./build/Debug/crt_debug: src/main.cpp include/utils.hpp build/Debug
	$(CXX) $(CXXFLAGS_DEBUG) src/main.cpp -o ./build/Debug/crt_debug

./build/Release/crt_release: src/main.cpp include/utils.hpp build/Release
	$(CXX) $(CXXFLAGS_RELEASE) src/main.cpp -o ./build/Release/crt_release

build/Debug:
	mkdir -p build/Debug

build/Release:
	mkdir -p build/Release

build:
	mkdir -p build

clean:
	rm -r build
