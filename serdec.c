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

#include "serdec.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define INDENT "\x20\x20\x20\x20"

struct serdec_json_list {
    struct serdec_json* head;
    struct serdec_json* tail;
    size_t length;
};

struct serdec_json_array {
    serdec_json_list_t* array;
};

struct serdec_json {
    char* key;
    serdec_json_type json_type;
    union {
        void* null;
        bool bool_val;
        int64_t int_val;
        double float_val;
        char* str_val;
        serdec_json_array_t* array;
        serdec_json_list_t* children;
    } value;
    struct serdec_json* next;
};

serdec_json_t* serdec_json_new_node(void) {
    serdec_json_t* node = malloc(sizeof(serdec_json_t));
    if (!node)
        return NULL;

    node->key = NULL;
    node->json_type = SERDEC_JSON_NULL;
    memset(&node->value, 0, sizeof(node->value));
    node->next = NULL;

    return node;
}

serdec_json_t* serdec_json_new_null(void) {

}

serdec_json_t* serdec_json_new_bool(bool value) {

}

serdec_json_t* serdec_json_new_int(int64_t value) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_INT;
    node->value.int_val = value;
    return node;
}

serdec_json_t* serdec_json_new_float(double value) {

}

serdec_json_t* serdec_json_new_string(const char* string) {

}

serdec_json_t* serdec_json_new_array(void) {

}

serdec_json_t* serdec_json_new_object(void) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_OBJECT;

    node->value.children = malloc(sizeof(serdec_json_list_t));
    if (!node->value.children) {
        free(node);
        return NULL;
    }
    
    node->value.children->head = NULL;
    node->value.children->tail = NULL;
    node->value.children->length = 0;

    return node;
}

bool serdec_json_add_null(serdec_json_t* object, const char* key, void* null) {

}

bool serdec_json_add_bool(serdec_json_t* object, const char* key, bool value) {

}

bool serdec_json_add_int(serdec_json_t* object, const char* key, int64_t value) {
    if (!object || object->json_type != SERDEC_JSON_OBJECT || !object->value.children)
        return false;

    serdec_json_t* new_node = serdec_json_new_int(value);
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);
    new_node->next = NULL;

    serdec_json_list_t* list = object->value.children;
    if (!list->head) {
        list->head = list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->length++;
    return true;
}

bool serdec_json_add_float(serdec_json_t* object, const char* key, double value) {

}

bool serdec_json_add_string(serdec_json_t* object, const char* key, const char* value) {

}

bool serdec_json_add_array(serdec_json_t* object, const char* key, serdec_json_array_t* value) {

}

bool serdec_json_add_object(serdec_json_t* object, const char* key, serdec_json_t* value) {

}

bool serdec_json_array_add(serdec_json_array_t* array, serdec_json_t* value) {

}

char* serdec_json_stringify(serdec_json_t* object) {
    serdec_json_t* ptr = object->value.children->head;
    if (!ptr)
        return NULL;

    if (ptr->json_type == SERDEC_JSON_INT) {
        char* string = malloc(100);
        strcpy(string, "{\n");
    
        while (ptr != NULL) {
            char tmp[100];
            snprintf(tmp, 100, INDENT "\"%s\": %" PRId64 "%s\n", ptr->key, ptr->value.int_val, (!ptr->next) ? "" : ",");
            strcat(string, tmp);
            ptr = ptr->next;
        }
        strcat(string, "}");
        return string;
    }

    return NULL;
}

serdec_json_t* serdec_json_parse(const char* json_string) {

}

void serdec_json_free(serdec_json_t* node) {

}