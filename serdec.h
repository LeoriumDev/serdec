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

#define sj_new_obj serdec_json_new_object
#define sj_stringify serdec_json_stringify

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
char* json_stringify(serdec_json_t* node);