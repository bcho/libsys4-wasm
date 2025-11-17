# sys4-validate WASM Test Binary

A WebAssembly test binary for validating ACX files using libsys4.

## Building

First, ensure you have the Emscripten SDK activated and libsys4.a built:

```bash
# Activate Emscripten SDK
source emsdk/emsdk_env.sh

# Build libsys4.a
./build_wasm.sh

# Build the test binary
chmod +x build_test_wasm.sh
./build_test_wasm.sh
```

This will generate:
- `build-wasm/test/sys4-validate.wasm` - The WebAssembly module
- `build-wasm/test/sys4-validate.js` - JavaScript glue code

## Usage

### Node.js

Use the provided test script:

```bash
node test/test-validate.js path/to/file.acx
```

Or use it programmatically:

```javascript
const createModule = require('./build-wasm/test/sys4-validate.js');
const fs = require('fs');

async function test() {
    const Module = await createModule();

    // Load file into WASM filesystem
    const fileData = fs.readFileSync('file.acx');
    Module.FS.writeFile('/tmp/file.acx', fileData);

    // Validate
    const result = Module.ccall('validate_acx', 'number', ['string'], ['/tmp/file.acx']);

    // Get results
    if (Module.ccall('get_success', 'number', [], [])) {
        console.log('Lines:', Module.ccall('get_nr_lines', 'number', [], []));
        console.log('Columns:', Module.ccall('get_nr_columns', 'number', [], []));
    } else {
        console.log('Error:', Module.ccall('get_error_message', 'string', [], []));
    }
}

test();
```

### Web Browser

1. Copy the generated files to your web directory:
   ```bash
   cp build-wasm/test/sys4-validate.* test/
   ```

2. Serve the `test/sys4-validate.html` file with a web server:
   ```bash
   python3 -m http.server 8000
   ```

3. Open http://localhost:8000/test/sys4-validate.html in your browser

4. Upload an ACX file and click "Validate"

## API Reference

### Functions

#### `validate_acx(path)`
Validates an ACX file at the given path (in WASM filesystem).

**Parameters:**
- `path` (string): Path to the ACX file

**Returns:**
- `0` on success
- Error code on failure

#### `get_success()`
Returns whether the last validation was successful.

**Returns:**
- `1` if successful
- `0` if failed

#### `get_nr_lines()`
Returns the number of lines in the validated ACX file.

**Returns:** Integer number of lines

#### `get_nr_columns()`
Returns the number of columns in the validated ACX file.

**Returns:** Integer number of columns

#### `get_error_code()`
Returns the error code from the last validation.

**Returns:**
- `0` (ACX_SUCCESS) on success
- `1` (ACX_ERROR_FILE) if file not found
- `2` (ACX_ERROR_INVALID) if file is invalid

#### `get_error_message()`
Returns a descriptive error message from the last validation.

**Returns:** String error message

#### `print_acx_info(path)`
Prints detailed information about an ACX file to stdout (useful for debugging).

**Parameters:**
- `path` (string): Path to the ACX file

## Example Output

Success:
```
✓ VALIDATION SUCCESSFUL
============================================================
Lines: 100
Columns: 5

Details: ACX file loaded successfully: 100 lines, 5 columns
```

Failure:
```
✗ VALIDATION FAILED
============================================================
Error Code: 1
Error: File not found or cannot be read: /tmp/missing.acx
```

## Notes

- Files must be loaded into the WASM filesystem before validation
- Use `Module.FS.writeFile()` to write files to the WASM filesystem
- The WASM module includes zlib for decompression support
- Memory grows automatically as needed
