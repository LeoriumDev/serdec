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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Serdec JSON function alias */
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


/* Initial size (in bytes) for the JSON serialization buffer. */
#define SERDEC_INITIAL_BUFFER_SIZE 256

/* Growth multiplier for the buffer when it runs out of space. */
#define SERDEC_BUFFER_GROWTH_FACTOR 2

/* Default indentation used when serializing formatted JSON. */
#define SERDEC_INDENT "\x20\x20\x20\x20"

/* Avoid C compiler automatically cast bool to int */
#define SERDEC_TO_BOOL(value) ((bool) value)

/* JSON field types */
typedef enum {
    SERDEC_JSON_NULL    ,
    SERDEC_JSON_BOOLEAN ,
    SERDEC_JSON_INT     ,
    SERDEC_JSON_FLOAT   ,
    SERDEC_JSON_STRING  ,
    SERDEC_JSON_ARRAY   ,
    SERDEC_JSON_OBJECT
} serdec_json_type;

/* JSON object structures */
typedef struct serdec_json_list  serdec_json_list_t;
typedef struct serdec_json       serdec_json_t;

/* JSON object creation functions */
serdec_json_t* serdec_json_new_node   (void);
serdec_json_t* serdec_json_new_null   (void);
serdec_json_t* serdec_json_new_bool   (bool value);
serdec_json_t* serdec_json_new_int    (int64_t value);
serdec_json_t* serdec_json_new_float  (double value);
serdec_json_t* serdec_json_new_string (const char* string);
serdec_json_t* serdec_json_new_array  (void);
serdec_json_t* serdec_json_new_object (void);

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
bool serdec_json_array_append(serdec_json_t* object, serdec_json_t* value);

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

#if __STDC_VERSION__ >= 201112L
#define serdec_json_array_push(array, value)                    \
    serdec_json_array_append(array,                             \
        _Generic((value),                                       \
            void*:                serdec_json_new_null,         \
            int:                  serdec_json_new_int,          \
            int64_t:              serdec_json_new_int,          \
            float:                serdec_json_new_float,        \
            double:               serdec_json_new_float,        \
            const char*:          serdec_json_new_string,       \
            char*:                serdec_json_new_string,       \
            bool:                 serdec_json_new_bool,         \
            serdec_json_t*:       (value),                      \
            default:              serdec_json_new_null          \
        )(value))
#endif

/* Utility */
char*          serdec_json_stringify (serdec_json_t* node);
serdec_json_t* serdec_json_parse     (const char* json_string);
void           serdec_json_free      (serdec_json_t* node);

#ifdef __cplusplus
}
#endif