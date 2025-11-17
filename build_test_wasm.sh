#!/bin/bash
# Build script for sys4-validate.wasm test binary

set -e

# Make sure Emscripten SDK is activated
if [ -z "$EMSDK" ]; then
    echo "Error: EMSDK environment variable not set."
    echo "Please activate Emscripten SDK first:"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    exit 1
fi

# Build directory
BUILD_DIR="build-wasm"
TEST_DIR="test"

# Make sure libsys4.a is built
if [ ! -f "$BUILD_DIR/libsys4.a" ]; then
    echo "Error: libsys4.a not found. Please run build_wasm.sh first."
    exit 1
fi

# Create test output directory
mkdir -p "$BUILD_DIR/test"

echo "Building sys4-validate.wasm..."

# Compile the test program
emcc "$TEST_DIR/sys4-validate.c" \
    -o "$BUILD_DIR/test/sys4-validate.wasm" \
    -I./include \
    -L./$BUILD_DIR \
    -lsys4 \
    -O3 \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_validate_acx","_get_nr_lines","_get_nr_columns","_get_error_code","_get_error_message","_get_success","_print_acx_info"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","FS"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='createSys4Validate' \
    -s ENVIRONMENT='web,node' \
    -s FILESYSTEM=1 \
    -lz

echo ""
echo "Build complete! Output files:"
echo "  - $BUILD_DIR/test/sys4-validate.wasm"
echo "  - $BUILD_DIR/test/sys4-validate.js"
echo ""
echo "Usage example (Node.js):"
echo "  const createModule = require('./build-wasm/test/sys4-validate.js');"
echo "  createModule().then(Module => {"
echo "    const result = Module.ccall('validate_acx', 'number', ['string'], ['/path/to/file.acx']);"
echo "    console.log('Lines:', Module.ccall('get_nr_lines', 'number', [], []));"
echo "  });"
