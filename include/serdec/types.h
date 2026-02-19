#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct SerdecBuffer   SerdecBuffer;
typedef struct SerdecArena    SerdecArena;
typedef struct SerdecDocument SerdecDocument;
typedef struct SerdecValue    SerdecValue;
typedef struct SerdecParser   SerdecParser;

/**
 * @brief A string slice pointing into the input buffer (borrowed by default).
 *
 * If has_escapes is false, ptr/len is already the final decoded value.
 * If has_escapes is true, call serdec_json_string_materialize() to decode.
 *
 * @note The input buffer must outlive this value.
 */
typedef struct {
    const char* ptr;
    size_t      len;
    bool        has_escapes;
} SerdecString;

/**
 * @brief Event kinds emitted by the JSON event iterator.
 */
typedef enum {
    SERDEC_EVENT_START_OBJECT,
    SERDEC_EVENT_END_OBJECT,
    SERDEC_EVENT_START_ARRAY,
    SERDEC_EVENT_END_ARRAY,
    SERDEC_EVENT_KEY,     /**< payload: string */
    SERDEC_EVENT_STRING,  /**< payload: string */
    SERDEC_EVENT_NUMBER,  /**< payload: string (raw slice, unparsed) */
    SERDEC_EVENT_BOOL,    /**< payload: boolean */
    SERDEC_EVENT_NULL,
    SERDEC_EVENT_ERROR,   /**< call serdec_json_parser_error() for details */
    SERDEC_EVENT_END,     /**< input exhausted, no more events */
} SerdecEventKind;

/**
 * @brief A single event produced by the JSON event iterator.
 */
typedef struct {
    SerdecEventKind kind;
    size_t          offset;  /**< byte offset of this event in the input */
    union {
        SerdecString string;  /**< KEY, STRING, NUMBER */
        bool         boolean; /**< BOOL */
    };
} SerdecEvent;
