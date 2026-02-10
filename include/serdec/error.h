#pragma once      

#include <stddef.h>

typedef enum SerdecError {
    SERDEC_OK = 0,

    // Syntax errors (100-199)
    SERDEC_ERR_UNEXPECTED_CHAR = 100,
    SERDEC_ERR_UNEXPECTED_EOF,
    SERDEC_ERR_INVALID_VALUE,
    SERDEC_ERR_TRAILING_CHARS,

    // String errors (200-299)
    SERDEC_ERR_INVALID_ESCAPE = 200,
    SERDEC_ERR_INVALID_UTF8,
    SERDEC_ERR_UNTERMINATED_STRING,

    // Number errors (300-399)
    SERDEC_ERR_INVALID_NUMBER = 300,
    SERDEC_ERR_NUMBER_OVERFLOW,

    // Resource errors (400-499)
    SERDEC_ERR_DEPTH_LIMIT = 400,
    SERDEC_ERR_MEMORY_LIMIT,
    SERDEC_ERR_OUT_OF_MEMORY,

    // I/O errors (500-599)
    SERDEC_ERR_IO = 500,
    SERDEC_ERR_FILE_NOT_FOUND,

    // Corrupted struct (600)
    SERDEC_ERR_INVALID_HANDLE = 600,
} SerdecError;

typedef struct SerdecErrorInfo {
    SerdecError code;
    size_t offset;
    size_t line;
    size_t column;
    char path[256];
    char message[512];
    char context[128];
} SerdecErrorInfo;

const char* serdec_error_string(SerdecError code);
void serdec_error_format(const SerdecErrorInfo *info, char *buf, size_t bufsize);
