#!/usr/bin/env bash

set -euo pipefail

# --- CONFIGURATION ---
BUILD_TYPE="${1:-release}" # Defaults to release if no argument is provided
SCENE_NAME="${2:-}"

if [ -z "${SCENE_NAME}" ]; then
    echo "Error: a scene name is required." >&2
    echo "Usage: $0 [debug|release] <CRT##-Scene#>" >&2
    echo "  e.g. $0 release CRT11-Scene0" >&2
    exit 1
fi

# Map the user argument to your exact Makefile targets and binary paths
if [ "${BUILD_TYPE,,}" = "debug" ]; then
    MAKE_TARGET="debug"
    BINARY_PATH="./build/Debug/crt_debug"
    OUTPUT_HTML="flamegraph_debug.html"
elif [ "${BUILD_TYPE,,}" = "release" ]; then
    MAKE_TARGET="release"
    BINARY_PATH="./build/Release/crt_release"
    OUTPUT_HTML="flamegraph_release.html"
else
    echo "Error: invalid build type '${BUILD_TYPE}'." >&2
    echo "Usage: $0 [debug|release] <CRT##-Scene#>" >&2
    echo "  e.g. $0 release CRT11-Scene0" >&2
    exit 1
fi

BINARY_ARGS="$(./getScenePath.sh "${SCENE_NAME}")"

# --- BUILD ---
echo "Building target: ${MAKE_TARGET}..."
make "${MAKE_TARGET}"

# Double check that the target successfully created the binary
if [ ! -f "$BINARY_PATH" ]; then
    echo "Error: Executable not found at $BINARY_PATH after running make." >&2
    exit 1
fi

# --- PROFILE ---
echo "Running application under perf..."
# -F 99: Samples at 99Hz to capture deep stacks without slowing the app down too much
sudo perf record -F 99 -g -- "$BINARY_PATH" ${BINARY_ARGS}

# Fix permissions so you don't need root to process the data
sudo chown $(id -u):$(id -g) perf.data

# --- GENERATE FLAMEGRAPH ---
echo "Processing perf data..."
perf script > out.perf

echo "Collapsing stack traces..."
stackcollapse-perf.pl out.perf > out.folded

echo "Generating flamegraph..."
flamegraph.pl out.folded > "${OUTPUT_HTML}"

# --- CLEANUP ---
rm -f out.perf out.folded

echo "Success! Open ${OUTPUT_HTML} in your web browser."
