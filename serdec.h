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

#ifdef __cplusplus
extern "C" {
#endif

#define sjson_add_null    serdec_json_add_null
#define sjson_add_bool    serdec_json_add_bool
#define sjson_add_int     serdec_json_add_int
#define sjson_add_float   serdec_json_add_float
#define sjson_add_string  serdec_json_add_string
#define sjson_add_array   serdec_json_add_array
#define sjson_add_object  serdec_json_add_object

typedef enum {
    SERDEC_JSON_NULL    ,
    SERDEC_JSON_BOOLEAN ,
    SERDEC_JSON_INT     ,
    SERDEC_JSON_FLOAT   ,
    SERDEC_JSON_STRING  ,
    SERDEC_JSON_ARRAY   ,
    SERDEC_JSON_OBJECT
} serdec_json_type;

typedef struct serdec_json_list  serdec_json_list_t;
typedef struct serdec_json_array serdec_json_array_t;
typedef struct serdec_json       serdec_json_t;

serdec_json_t* serdec_json_new_object(void);

bool serdec_json_add_null   (serdec_json_t* object, const char* key, void* null);
bool serdec_json_add_bool   (serdec_json_t* object, const char* key, bool value);
bool serdec_json_add_int    (serdec_json_t* object, const char* key, int64_t value);
bool serdec_json_add_float  (serdec_json_t* object, const char* key, double value);
bool serdec_json_add_string (serdec_json_t* object, const char* key, const char* value);
bool serdec_json_add_array  (serdec_json_t* object, const char* key, serdec_json_array_t* value);
bool serdec_json_add_object (serdec_json_t* object, const char* key, serdec_json_t* value);

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
        serdec_json_array_t*: serdec_json_add_array,       \
        serdec_json_t*:       serdec_json_add_object,      \
        default:              serdec_json_add_null         \
    )(object, key, value)
#endif

char* serdec_json_stringify(serdec_json_t* node);

#ifdef __cplusplus
}
#endif