#!/bin/bash
# Build script for WebAssembly using Emscripten

set -e

# Make sure Emscripten SDK is activated
if [ -z "$EMSDK" ]; then
    echo "Error: EMSDK environment variable not set."
    echo "Please activate Emscripten SDK first:"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

# Create build directory
BUILD_DIR="build-wasm"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake using Emscripten
emcmake cmake .. \
    -DEMSCRIPTEN=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_FLAGS="-O3"

# Build
emmake make -j$(nproc)

echo ""
echo "Build complete! libsys4.a is in $BUILD_DIR/"
