.DEFAULT_GOAL := help
help:
	@echo "Specify a target: make debug | make release | make clean"

DEFINITIONS ?=
WERROR ?= -Werror

CXX      = g++
CXXFLAGS = -std=c++26 -I./include/ -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion $(DEFINITIONS)

CXXFLAGS_DEBUG   = $(CXXFLAGS) -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -DDEBUG
CXXFLAGS_RELEASE = $(CXXFLAGS) -g -O3 -flto=auto $(WERROR) -DNDEBUG

SRCS := $(wildcard src/*.cpp)

OBJS_DEBUG   := $(patsubst src/%.cpp, build/Debug/%.o, $(SRCS))
OBJS_RELEASE := $(patsubst src/%.cpp, build/Release/%.o, $(SRCS))

DEPS_DEBUG   := $(OBJS_DEBUG:.o=.d)
DEPS_RELEASE := $(OBJS_RELEASE:.o=.d)

debug: ./build/Debug/crt_debug
release: ./build/Release/crt_release

./build/Debug/crt_debug: $(OBJS_DEBUG) | build/Debug
	$(CXX) $(CXXFLAGS_DEBUG) $(OBJS_DEBUG) -o $@

./build/Release/crt_release: $(OBJS_RELEASE) | build/Release
	$(CXX) $(CXXFLAGS_RELEASE) $(OBJS_RELEASE) -o $@

build/Debug/%.o: src/%.cpp | build/Debug
	$(CXX) $(CXXFLAGS_DEBUG) -MMD -MP -c $< -o $@

build/Release/%.o: src/%.cpp | build/Release
	$(CXX) $(CXXFLAGS_RELEASE) -MMD -MP -c $< -o $@

build/Debug:
	mkdir -p $@

build/Release:
	mkdir -p $@

build:
	mkdir -p $@

clean:
	rm -rf build

-include $(DEPS_DEBUG)
-include $(DEPS_RELEASE)

.PHONY: debug release clean help build
