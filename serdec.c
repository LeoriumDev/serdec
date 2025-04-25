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


serdec_json_t* serdec_json_new_object(void) {
    serdec_json_t* node = malloc(sizeof(serdec_json_t));
    if (!node)
        return NULL;

    node->key = NULL;
    node->json_type = SERDEC_JSON_OBJECT;
    
    node->value.children = malloc(sizeof(serdec_json_list_t));
    node->value.children->head = NULL;
    node->value.children->tail = NULL;
    node->value.children->length = 0;

    node->next = NULL;
    return node;
}

serdec_json_t* serdec_json_new_int(int64_t value) {
    serdec_json_t* node = malloc(sizeof(serdec_json_t));
    if (!node)
        return NULL;

    node->key = NULL;
    node->json_type = SERDEC_JSON_INT;
    node->value.int_val = value;
    node->next = NULL;
    return node;
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

char* serdec_json_stringify(serdec_json_t* object) {
    char* string = malloc(100);
    strcat(string, "{\n");
    serdec_json_t* ptr = object->value.children->head;
    while(ptr != NULL) {
        char tmp[100];
        snprintf(tmp, 100, INDENT "\"%s\": %" PRId64 "%s\n", ptr->key, ptr->value.int_val, (!ptr->next) ? "" : ",");
        strcat(string, tmp);
        ptr = ptr->next;
    }
    strcat(string,  "}");
    return string;
}