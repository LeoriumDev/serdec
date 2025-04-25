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

/**
 * json_create_object() - Create a new, empty JSON object
 *
 * Allocates and initializes a new, empty JSON object.
 *
 * Returns:
 *   NULL on memory allocation failure.
 *   The caller must manually free it with json_free_object().
 */
serdec_json_t* serdec_json_new_object(void);

// /**
//  * json_free_object() - Free a JSON object
//  *
//  * Releases all memory associated with the JSON object and its members.
//  *
//  * @object: A pointer to the JSON object to free
//  */
// void json_free_object(serdec_json* object);

/**
 * json_stringify() - Serialize a JSON object to string
 *
 * Converts a JSON object into a string representation. The returned
 * string is dynamically allocated and must be freed by the caller.
 *
 * @object: A pointer to the JSON object to be serialized
 *
 * Returns:
 *   NULL on empty or NULL object.
 */
char* json_stringify(serdec_json_t* node);

// /**
//  * json_add_object() - Add a child JSON object to a parent JSON object
//  *
//  * Adds a key-value pair to a JSON object, where the value is another
//  * JSON object (a child).
//  *
//  * @object: pointer to the parent JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @val: A pointer to JSON object to add
//  *
//  * Returns:
//  *   false if object, key, or val is NULL, or if memory allocation fails.
//  */
// bool json_add_object(serdec_json* object, const char* key, serdec_json* val);

// /**
//  * json_add_array() - Add an string array to a JSON object
//  *
//  * Adds a key-value pair to a JSON object, where the value is a string array.
//  *
//  * @object: pointer to the JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @val: A pointer to JSON array to add
//  *
//  * Returns:
//  *   false if object, key, or val is NULL, or if memory allocation fails.
//  */
// bool json_add_array(serdec_json* object, const char* key, serdec_json* val);

// /**
//  * json_add_int() - Add an integer to a JSON object
//  *
//  * Adds a key-value pair to a JSON object, where the value is an integer.
//  *
//  * @object: pointer to a JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @val: An interger to add
//  *
//  * Returns:
//  *   false if object, key is NULL, or if memory allocation fails.
//  */
// bool json_add_int(serdec_json* object, char* key, int val);

// /**
//  * json_add_str() - Add a string to a JSON object
//  *
//  * Adds a key-value pair to a JSON object, where the value is a string.
//  *
//  * @object: pointer to the parent JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @val: A pointer to a string to add
//  *
//  * Returns:
//  *   false if object, key, or val is NULL, or if memory allocation fails.
//  */
// bool json_add_str(serdec_json* object, const char* key, const char* val);

// /**
//  * json_add_bool() - Add a boolean to a JSON object
//  *
//  * Adds a key-value pair to a JSON object, where the value is another
//  * JSON object (a child).
//  *
//  * @object: pointer to JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @val: A boolean value to add
//  *
//  * Returns:
//  *   false if object, key, or val is NULL, or if memory allocation fails.
//  */
// bool json_add_bool(serdec_json* object, const char* key, bool val);

// /**
//  * json_add_null() - Add a child JSON object to a parent JSON object
//  *
//  * Adds a key-value pair to a JSON object, where the value is another
//  * JSON object (a child).
//  *
//  * @object: pointer to the parent JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @val: this value is discarded
//  *
//  * Returns:
//  *   false if object, key, or if memory allocation fails.
//  */
// bool json_add_null(serdec_json* object, const char* key, const char* val);

// /**
//  * json_add() - A generic macro to add values to a JSON object
//  *
//  * This macro uses the _Generic keyword to select the appropriate add
//  * function based on the type of the 'value' argument.
//  *
//  * @object: pointer to the parent JSON object
//  * @key: A string representing the key for the new key-value pair
//  * @value: The value to add.  Must be one of the supported types
//  *         (JSON*, JSON_array*, int, char*, bool, null).
//  *
//  * Returns:
//  *   false if object, key, or val is NULL, or if memory allocation fails.
//  */
// #define json_add(object, key, value)                \
//     _Generic((value),                               \
//         JSON*: json_add_object,                     \
//         JSON_array*: json_add_array,                \
//         int: json_add_int,                          \
//         char*: json_add_str,                        \
//         bool: json_add_bool,                        \
//         default: json_add_null                      \
//     )(object, key, value)

// /**
//  * to_json_array() - Convert a char array to a JSON_array struct
//  *
//  * This function creates a JSON_array struct that encapsulates a character
//  * array.
//  *
//  * @array: A pointer to a character array
//  *
//  * Returns:
//  *   NULL if conversion fails.
//  */
// // JSON_array* to_json_array(char* array);

// bool json_write(serdec_json* object, char* filename);