/* Copyright (C) 2024
 *
 * Test WASM binary for validating ACX files using libsys4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#include "system4/acx.h"
#include "system4/string.h"

// Structure to hold validation results
typedef struct {
    int success;
    int error_code;
    int nr_lines;
    int nr_columns;
    char error_message[256];
} ValidationResult;

static ValidationResult last_result = {0};

/**
 * Validate an ACX file and return detailed information
 *
 * @param path Path to the ACX file to validate
 * @return 0 on success, error code otherwise
 */
EMSCRIPTEN_KEEPALIVE
int validate_acx(const char *path) {
    int error = 0;
    struct acx *acx = NULL;

    memset(&last_result, 0, sizeof(ValidationResult));

    if (!path || strlen(path) == 0) {
        last_result.success = 0;
        last_result.error_code = -1;
        snprintf(last_result.error_message, sizeof(last_result.error_message),
                 "Invalid path provided");
        return -1;
    }

    // Load the ACX file
    acx = acx_load(path, &error);

    if (!acx || error != ACX_SUCCESS) {
        last_result.success = 0;
        last_result.error_code = error;

        switch (error) {
            case ACX_ERROR_FILE:
                snprintf(last_result.error_message, sizeof(last_result.error_message),
                         "File not found or cannot be read: %s", path);
                break;
            case ACX_ERROR_INVALID:
                snprintf(last_result.error_message, sizeof(last_result.error_message),
                         "Invalid ACX file format: %s", path);
                break;
            default:
                snprintf(last_result.error_message, sizeof(last_result.error_message),
                         "Unknown error (%d) loading file: %s", error, path);
                break;
        }
        return error;
    }

    // Successfully loaded
    last_result.success = 1;
    last_result.error_code = ACX_SUCCESS;
    last_result.nr_lines = acx->nr_lines;
    last_result.nr_columns = acx->nr_columns;
    snprintf(last_result.error_message, sizeof(last_result.error_message),
             "ACX file loaded successfully: %d lines, %d columns",
             last_result.nr_lines, last_result.nr_columns);

    // Clean up
    acx_free(acx);

    return 0;
}

/**
 * Get the number of lines from the last validation
 */
EMSCRIPTEN_KEEPALIVE
int get_nr_lines(void) {
    return last_result.nr_lines;
}

/**
 * Get the number of columns from the last validation
 */
EMSCRIPTEN_KEEPALIVE
int get_nr_columns(void) {
    return last_result.nr_columns;
}

/**
 * Get the error code from the last validation
 */
EMSCRIPTEN_KEEPALIVE
int get_error_code(void) {
    return last_result.error_code;
}

/**
 * Get the error message from the last validation
 */
EMSCRIPTEN_KEEPALIVE
const char* get_error_message(void) {
    return last_result.error_message;
}

/**
 * Get validation success status
 */
EMSCRIPTEN_KEEPALIVE
int get_success(void) {
    return last_result.success;
}

/**
 * Print ACX file information (for debugging)
 */
EMSCRIPTEN_KEEPALIVE
void print_acx_info(const char *path) {
    int error = 0;
    struct acx *acx = acx_load(path, &error);

    if (!acx) {
        printf("Failed to load ACX file: %s (error: %d)\n", path, error);
        return;
    }

    printf("ACX File: %s\n", path);
    printf("Lines: %d\n", acx->nr_lines);
    printf("Columns: %d\n", acx->nr_columns);

    // Print first few lines as sample
    int max_lines = acx->nr_lines < 5 ? acx->nr_lines : 5;
    for (int line = 0; line < max_lines; line++) {
        printf("Line %d: ", line);
        for (int col = 0; col < acx->nr_columns; col++) {
            enum acx_column_type type = acx->column_types[col];
            if (type == ACX_INT) {
                printf("%d", acx_get_int(acx, line, col));
            } else if (type == ACX_STRING) {
                struct string *s = acx_get_string(acx, line, col);
                printf("\"%s\"", s->text);
            }
            if (col < acx->nr_columns - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }

    acx_free(acx);
}

int main(int argc, char *argv[]) {
    printf("sys4-validate WASM module loaded\n");
    printf("Available functions:\n");
    printf("  - validate_acx(path): Validate an ACX file\n");
    printf("  - get_nr_lines(): Get number of lines from last validation\n");
    printf("  - get_nr_columns(): Get number of columns from last validation\n");
    printf("  - get_error_code(): Get error code from last validation\n");
    printf("  - get_error_message(): Get error message from last validation\n");
    printf("  - print_acx_info(path): Print detailed ACX file information\n");
    return 0;
}
