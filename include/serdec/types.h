#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Opaque data type
typedef struct SerdecBuffer SerdecBuffer;
typedef struct SerdecArena SerdecArena;
typedef struct SerdecDocument SerdecDocument;
typedef struct SerdecValue SerdecValue;

// Error codes
typedef enum SerdecError {
    SERDEC_OK = 0,

    // Syntax errors (100-199)
    SERDEC_ERR_UNEXPECTED_CHAR = 100,
    SERDEC_ERR_UNEXPECTED_EOF,
    SERDEC_ERR_INVALID_VALUE,

    // String errors (200-299)
    SERDEC_ERR_INVALID_ESCAPE = 200,
    SERDEC_ERR_INVALID_UTF8,

    // Number errors (300-399)
    SERDEC_ERR_INVALID_NUMBER = 300,
    SERDEC_ERR_NUMBER_OVERFLOW,

    // Resource errors (400-499)
    SERDEC_ERR_DEPTH_LIMIT = 400,
    SERDEC_ERR_MEMORY_LIMIT,
    SERDEC_ERR_OUT_OF_MEMORY,
} SerdecError;

// Error information
typedef struct SerdecErrorInfo {
    SerdecError code;
    size_t offset;
    size_t line;
    size_t column;
    char message[256];
} SerdecErrorInfo;
