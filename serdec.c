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
    if (object != NULL)

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

    JSON* tmp = json_create_object();
    object->next = tmp;
    object = object->next;
    return true;
}