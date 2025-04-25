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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "serdec.h"

#define INDENT "\x20\x20\x20\x20"

JSON* json_create_object(void) {
    JSON* object = malloc(sizeof(JSON));
    object->next = object;
    object->prev = object;
    object->child = NULL;
    return object;
}

void json_free_object(JSON* object) { free(object); }

char* json_stringify(JSON* object) {
    if (object->type == JSON_INT) {
        char buf[100] = {};
        sprintf(buf, "%d", object->value.int_val);

        size_t total_len = strlen("{\n\08\08\08\08") + strlen(object->key) +
                           strlen("\": ") + strlen(buf) + strlen("\n}") + 10;

        char* string = malloc(total_len);
        if (!string)
            return NULL;

        snprintf(string, total_len, "{\n"INDENT"\"%s\": %s\n}", object->key, buf);
        return string;
    }

    return NULL;
}

bool json_write(JSON *object, char* filename) {
    FILE* file;
    file = fopen(filename, "w");
    if (!file) {
        perror(filename);
        return false;
    }

    char* json_data = json_stringify(object);
    if (!json_data) {
        fclose(file);
        return false;
    }

    fprintf(file, "%s", json_data);
    fclose(file);
    return true;
}

bool json_add_int(JSON *object, char *key, int value) {
    object->type = JSON_INT;
    object->value.int_val = value;
    object->key = malloc(strlen(key) + 1);
    strcpy(object->key, key);
    return true;
}