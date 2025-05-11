/**
 * Serdec - A C serialization and deserialization library
 * Version: 1.0.0
 * Copyright (c) 2025 Leorium <contact@leorium.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 * 
 * SPDX-License-Identifier: MIT
 */

/**
 * @file serdec_json.h
 * @brief JSON serialization and deserialization API for Serdec.
 *
 * This header defines the core API for creating, modifying, serializing,
 * and parsing (deserializing) JSON data structures in Serdec. All memory 
 * is managed via arenas for efficient allocation and deallocation.
 * 
 */

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Serdec JSON function alias */
#define SERDEC_JSON_ALLOW_ALIASING true

#if SERDEC_JSON_ALLOW_ALIASING
#define sjson_new_null    serdec_json_new_null
#define sjson_new_bool    serdec_json_new_bool
#define sjson_new_int     serdec_json_new_int
#define sjson_new_float   serdec_json_new_float
#define sjson_new_string  serdec_json_new_string
#define sjson_new_array   serdec_json_new_array
#define sjson_new_object  serdec_json_new_object

#define sjson_add_null       serdec_json_add_null
#define sjson_add_bool       serdec_json_add_bool
#define sjson_add_int        serdec_json_add_int
#define sjson_add_float      serdec_json_add_float
#define sjson_add_string     serdec_json_add_string
#define sjson_add_array      serdec_json_add_array
#define sjson_add_object     serdec_json_add_object

#define sjson_list_append    serdec_json_list_append
#define sjson_array_append   serdec_json_array_append
#endif

/* Initial size (in bytes) for the JSON serialization buffer. */
#define SERDEC_DEFAULT_BUFFER_CAP 256

#define SERDEC_INITIAL_ARRAY_CAP 8
#define SERDEC_INITIAL_OBJECT_CAP 8
/* Growth multiplier for the buffer when it runs out of space. */
#define SERDEC_BUFFER_GROWTH_FACTOR 2

/* Default indentation used when serializing formatted JSON. */
#define SERDEC_INDENT "\x20\x20\x20\x20"

/* Avoid C compiler automatically cast bool to int */
#define SERDEC_TO_BOOL(value) ((bool) value)

/* Dynamic Buffer */
// typedef struct serdec_buffer serdec_buffer_t;

/* JSON object structure */
typedef struct serdec_json        serdec_json_t;

/* JSON object creation functions */
serdec_json_t* serdec_json_new_root   (void);
serdec_json_t* serdec_json_new_node   (serdec_json_t* root, const char* key);
serdec_json_t* serdec_json_new_null   (serdec_json_t* root, const char* key);
serdec_json_t* serdec_json_new_bool   (serdec_json_t* root, const char* key, bool value);
serdec_json_t* serdec_json_new_int    (serdec_json_t* root, const char* key, int64_t value);
serdec_json_t* serdec_json_new_float  (serdec_json_t* root, const char* key, double value);
serdec_json_t* serdec_json_new_string (serdec_json_t* root, const char* key, const char* string);
serdec_json_t* serdec_json_new_array  (serdec_json_t* root, const char* key);
serdec_json_t* serdec_json_new_object (serdec_json_t* root, const char* key);

/* JSON object field appending functions */

/** 
 * The 'null' parameter exists to match the signature used by generic dispatch;
 * it is ignored during execution.
 */
bool serdec_json_add_null      (serdec_json_t* object, const char* key,
                                void* null);
bool serdec_json_add_bool      (serdec_json_t* object, const char* key,
                                bool value);
bool serdec_json_add_int       (serdec_json_t* object, const char* key,
                                int64_t value);
bool serdec_json_add_float     (serdec_json_t* object, const char* key,
                                double value);
bool serdec_json_add_string    (serdec_json_t* object, const char* key,
                                const char* value);
bool serdec_json_add_object    (serdec_json_t* object, const char* key,
                                serdec_json_t* value);
bool serdec_json_add_array     (serdec_json_t* object, const char* key,
                                serdec_json_t* value);


/* Append a value node to the internal list of an object or array. */
bool serdec_json_list_append(serdec_json_t* object, serdec_json_t* value);

/* Append a value node to a JSON array. */
// void serdec_json_array_push(serdec_json_t* object, serdec_json_t* value);

/* 
 * Automatically dispatch to the correct serdec_json_add_* function based on
 * value type (C11 _Generic).
 */
#if __STDC_VERSION__ >= 201112L
#define serdec_json_add(object, key, value)                \
    _Generic((value),                                      \
        void*:                serdec_json_add_null,        \
        int:                  serdec_json_add_int,         \
        int64_t:              serdec_json_add_int,         \
        float:                serdec_json_add_float,       \
        double:               serdec_json_add_float,       \
        const char*:          serdec_json_add_string,      \
        char*:                serdec_json_add_string,      \
        bool:                 serdec_json_add_bool,        \
        serdec_json_t*:       serdec_json_add_object,      \
        default:              serdec_json_add_null         \
    )(object, key, value)
#endif

/* JSON Object Stringify functions */
char* serdec_json_stringify           (serdec_json_t* object);
char* serdec_json_stringify_raw       (serdec_json_t* object);
char* serdec_json_stringify_internal  (serdec_json_t* object, bool pretty_print);
char* serdec_json_stringify_primitive (serdec_json_t* object, size_t indent_level, bool pretty_print);
char* serdec_json_stringify_array     (serdec_json_t* object, size_t indent_level, bool pretty_print);
char* serdec_json_stringify_object    (serdec_json_t* object, size_t indent_level, bool pretty_print);

/* JSON object parser */
serdec_json_t* serdec_json_parse(const char* json_string);

/* release memory allocated in JSON object */
void serdec_json_free(serdec_json_t* node);

__attribute__((used))
static inline serdec_json_t* serdec_json_object_return(serdec_json_t* object) {
    return object;
}

#ifdef __cplusplus
}
#endif
