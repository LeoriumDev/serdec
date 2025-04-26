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

#define sjson_new_object serdec_json_new_object
#define sjson_stringify serdec_json_stringify
#define sjson_free serdec_json_free

typedef enum {
    SERDEC_JSON_NULL,
    SERDEC_JSON_BOOLEAN,
    SERDEC_JSON_INT,
    SERDEC_JSON_FLOAT,
    SERDEC_JSON_STRING,
    SERDEC_JSON_ARRAY,
    SERDEC_JSON_OBJECT
} serdec_json_type;

typedef struct serdec_json_list {
    struct serdec_json* head;
    struct serdec_json* tail;
    size_t length;
} serdec_json_list_t;

typedef struct serdec_json {
    char* key;
    serdec_json_type json_type;
    union {
        bool boolean_val;
        int64_t int_val;
        double float_val;
        char* string_val;
        serdec_json_list_t* children;
    } value;
    struct serdec_json* next;
} serdec_json_t;

serdec_json_t* serdec_json_new_object(void);
char* serdec_json_stringify(serdec_json_t* node);

bool serdec_json_add_null(serdec_json_t* object, const char* key);
bool serdec_json_add_bool(serdec_json_t* object, const char* key, int64_t value);
bool serdec_json_add_int(serdec_json_t* object, const char* key, int64_t value);
bool serdec_json_add_float(serdec_json_t* object, const char* key, double value);
bool serdec_json_add_string(serdec_json_t* object, const char* key, int64_t value);
bool serdec_json_add_array(serdec_json_t* object, const char* key, int64_t value);
bool serdec_json_add_object(serdec_json_t* object, const char* key, int64_t value);

#define serdec_json_add(object, key, value)             \
    _Generic((value),                                   \
        int:         serdec_json_object_add_int,         \
        int64_t:     serdec_json_object_add_int,         \
        float:       serdec_json_object_add_float,       \
        double:      serdec_json_object_add_float_wrap,       \
        const char*: serdec_json_object_add_str_or_null_wrap, \
        char*:       serdec_json_object_add_str_or_null_wrap, \
        bool:        serdec_json_object_add_bool_wrap,        \
        default:     serdec_json_object_add          \
    )(obj, key, value)
void serdec_json_free(serdec_json_t* root);
bool serdec_json_list_append(serdec_json_list_t* list, serdec_json_t* value);