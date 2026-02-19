#pragma once

#include <stddef.h>

/**
 * @brief Error codes returned by serdec functions.
 */
typedef enum SerdecError {
    SERDEC_OK = 0,

    // Syntax errors (100-199)
    SERDEC_ERR_UNEXPECTED_CHAR = 100,  /**< Unexpected character in input. */
    SERDEC_ERR_UNEXPECTED_EOF,         /**< Input ended before value was complete. */
    SERDEC_ERR_INVALID_VALUE,          /**< Value did not match any JSON type. */
    SERDEC_ERR_TRAILING_CHARS,         /**< Non-whitespace characters after root value. */

    // String errors (200-299)
    SERDEC_ERR_INVALID_ESCAPE = 200,   /**< Unknown or malformed escape sequence. */
    SERDEC_ERR_INVALID_UTF8,           /**< Invalid UTF-8 byte sequence. */
    SERDEC_ERR_UNTERMINATED_STRING,    /**< String was not closed before end of input. */

    // Number errors (300-399)
    SERDEC_ERR_INVALID_NUMBER = 300,   /**< Number did not conform to RFC 8259. */
    SERDEC_ERR_NUMBER_OVERFLOW,        /**< Number exceeded representable range. */

    // Resource errors (400-499)
    SERDEC_ERR_DEPTH_LIMIT = 400,      /**< Nesting depth exceeded configured limit. */
    SERDEC_ERR_MEMORY_LIMIT,           /**< Total memory exceeded configured limit. */
    SERDEC_ERR_OUT_OF_MEMORY,          /**< Allocation failed. */

    // I/O errors (500-599)
    SERDEC_ERR_IO = 500,               /**< Generic I/O error. */
    SERDEC_ERR_FILE_NOT_FOUND,         /**< File could not be opened. */

    // Internal errors (600)
    SERDEC_ERR_INVALID_HANDLE = 600,   /**< Corrupted or invalid struct passed. */
} SerdecError;

/**
 * @brief Detailed error information including location and message.
 */
typedef struct SerdecErrorInfo {
    SerdecError code;      /**< Error code. */
    size_t      offset;    /**< Byte offset in input where the error occurred. */
    size_t      line;      /**< Line number (1-indexed). */
    size_t      column;    /**< Column number (1-indexed). */
    char        path[256]; /**< Source path, if applicable. */
    char        message[512]; /**< Human-readable error message. */
    char        context[128]; /**< Snippet of input around the error. */
} SerdecErrorInfo;

/**
 * @brief Return a short string description of an error code.
 *
 * @param code Error code.
 * @return Null-terminated string. Valid for the lifetime of the process.
 */
const char* serdec_error_string(SerdecError code);

/**
 * @brief Format a full error report into a buffer.
 *
 * @param info    Error info to format.
 * @param buf     Output buffer.
 * @param bufsize Size of output buffer in bytes.
 */
void serdec_error_format(const SerdecErrorInfo* info, char* buf, size_t bufsize);
