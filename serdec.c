/**
 * Copyright (c) 2025 Leorium <contact@leorium.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include "serdec.h"

JSON* json_create_object(void) {
    JSON* object = malloc(sizeof(JSON));
    object->next = object;
    object->prev = object;
    object->child = NULL;
    return object;
}

void json_free_object(JSON* object) { free(object); }