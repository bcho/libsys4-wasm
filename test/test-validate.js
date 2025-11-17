#!/usr/bin/env node
/**
 * Node.js example for using sys4-validate.wasm
 *
 * Usage: node test-validate.js <path-to-acx-file>
 */

const fs = require('fs');
const path = require('path');

// Load the WASM module
const createModule = require('../build-wasm/test/sys4-validate.js');

async function validateACX(filePath) {
    console.log('Loading WASM module...');
    const Module = await createModule();
    console.log('WASM module loaded successfully!\n');

    // Check if file exists
    if (!fs.existsSync(filePath)) {
        console.error('Error: File not found:', filePath);
        process.exit(1);
    }

    // Read the file
    const fileData = fs.readFileSync(filePath);
    const fileName = '/tmp/' + path.basename(filePath);

    console.log('Processing file:', filePath);
    console.log('File size:', fileData.length, 'bytes\n');

    // Write file to WASM filesystem
    Module.FS.writeFile(fileName, fileData);

    // Validate the ACX file
    const result = Module.ccall('validate_acx', 'number', ['string'], [fileName]);

    // Get validation results
    const success = Module.ccall('get_success', 'number', [], []);
    const errorCode = Module.ccall('get_error_code', 'number', [], []);
    const errorMessage = Module.ccall('get_error_message', 'string', [], []);

    console.log('='.repeat(60));
    if (success) {
        const nrLines = Module.ccall('get_nr_lines', 'number', [], []);
        const nrColumns = Module.ccall('get_nr_columns', 'number', [], []);

        console.log('✓ VALIDATION SUCCESSFUL');
        console.log('='.repeat(60));
        console.log('Lines:', nrLines);
        console.log('Columns:', nrColumns);
        console.log('\nDetails:', errorMessage);

        // Print detailed info
        console.log('\n' + '='.repeat(60));
        console.log('DETAILED INFORMATION:');
        console.log('='.repeat(60));
        Module.ccall('print_acx_info', null, ['string'], [fileName]);

        process.exit(0);
    } else {
        console.log('✗ VALIDATION FAILED');
        console.log('='.repeat(60));
        console.log('Error Code:', errorCode);
        console.log('Error:', errorMessage);
        process.exit(1);
    }
}

// Main
if (process.argv.length < 3) {
    console.log('Usage: node test-validate.js <path-to-acx-file>');
    process.exit(1);
}

const filePath = process.argv[2];
validateACX(filePath).catch(err => {
    console.error('Error:', err);
    process.exit(1);
});
