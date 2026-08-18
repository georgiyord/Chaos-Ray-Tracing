.DEFAULT_GOAL := help
help:
	@echo "Specify a target: make debug | make release | make releaseWithSymbols | make clean"
	@echo "Library targets:  make libDebug | make libRelease | make libReleaseWithSymbols"

DEFINITIONS ?=
WERROR ?= -Werror

CXX      = g++
CXXFLAGS = -std=c++26 -I./include/ -fPIC -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion $(DEFINITIONS)

CXXFLAGS_DEBUG   = $(CXXFLAGS) -g -O0 -fsanitize=address,undefined -fno-omit-frame-pointer -DDEBUG
CXXFLAGS_RELEASE_SYMBOLS = $(CXXFLAGS) -g -O3 -march=native -flto=auto $(WERROR) -DNDEBUG
CXXFLAGS_RELEASE = $(CXXFLAGS) -s -O3 -march=native -flto=auto $(WERROR) -DNDEBUG

SRCS := $(wildcard src/*.cpp)

OBJS_DEBUG   := $(patsubst src/%.cpp, build/Debug/%.o, $(SRCS))
OBJS_RELEASE := $(patsubst src/%.cpp, build/Release/%.o, $(SRCS))
OBJS_RELEASE_SYMBOLS := $(patsubst src/%.cpp, build/ReleaseWithSymbols/%.o, $(SRCS))

LIB_OBJS_DEBUG   := $(filter-out build/Debug/main.o, $(OBJS_DEBUG))
LIB_OBJS_RELEASE := $(filter-out build/Release/main.o, $(OBJS_RELEASE))
LIB_OBJS_RELEASE_SYMBOLS := $(filter-out build/ReleaseWithSymbols/main.o, $(OBJS_RELEASE_SYMBOLS))

DEPS_DEBUG   := $(OBJS_DEBUG:.o=.d)
DEPS_RELEASE := $(OBJS_RELEASE:.o=.d)
DEPS_RELEASE_SYMBOLS := $(OBJS_RELEASE_SYMBOLS:.o=.d)

debug: ./build/Debug/crt_debug
release: ./build/Release/crt_release
releaseWithSymbols: ./build/ReleaseWithSymbols/crt_release

libDebug: build/Debug/libRenderEngine.so
libRelease: build/Release/libRenderEngine.so
libReleaseWithSymbols: build/ReleaseWithSymbols/libRenderEngine.so

./build/Debug/crt_debug: $(OBJS_DEBUG) | build/Debug
	$(CXX) $(CXXFLAGS_DEBUG) $(OBJS_DEBUG) -o $@

./build/Release/crt_release: $(OBJS_RELEASE) | build/Release
	$(CXX) $(CXXFLAGS_RELEASE) $(OBJS_RELEASE) -o $@

./build/ReleaseWithSymbols/crt_release: $(OBJS_RELEASE_SYMBOLS) | build/Release
	$(CXX) $(CXXFLAGS_RELEASE_SYMBOLS) $(OBJS_RELEASE_SYMBOLS) -o $@

build/Debug/libRenderEngine.so: $(LIB_OBJS_DEBUG) | build/Debug
	$(CXX) $(CXXFLAGS_DEBUG) -shared -o $@ $(LIB_OBJS_DEBUG)

build/Release/libRenderEngine.so: $(LIB_OBJS_RELEASE) | build/Release
	$(CXX) $(CXXFLAGS_RELEASE) -shared -o $@ $(LIB_OBJS_RELEASE)

build/ReleaseWithSymbols/libRenderEngine.so: $(LIB_OBJS_RELEASE_SYMBOLS) | build/ReleaseWithSymbols
	$(CXX) $(CXXFLAGS_RELEASE_SYMBOLS) -shared -o $@ $(LIB_OBJS_RELEASE_SYMBOLS)

build/Debug/%.o: src/%.cpp | build/Debug
	$(CXX) $(CXXFLAGS_DEBUG) -MMD -MP -c $< -o $@

build/Release/%.o: src/%.cpp | build/Release
	$(CXX) $(CXXFLAGS_RELEASE) -MMD -MP -c $< -o $@

build/ReleaseWithSymbols/%.o: src/%.cpp | build/ReleaseWithSymbols
	$(CXX) $(CXXFLAGS_RELEASE_SYMBOLS) -MMD -MP -c $< -o $@

build/Debug:
	mkdir -p $@

build/Release:
	mkdir -p $@

build/ReleaseWithSymbols:
	mkdir -p $@

build:
	mkdir -p $@

clean:
	rm -rf build

-include $(DEPS_DEBUG)
-include $(DEPS_RELEASE)
-include $(DEPS_RELEASE_SYMBOLS)

.PHONY: debug release releaseWithSymbols libDebug libRelease libReleaseWithSymbols clean help
