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
#include <string.h>

#include "serdec_arena.h"
#include "serdec_vector.h"
#include "serdec_string.h"

#define SERDEC_STRING_INITIAL_CAPACITY 16

serdec_string_t* serdec_string_new(const char* str, serdec_arena_t* arena) {
    if (!arena)
        return NULL;

    size_t len = str ? strlen(str) : 0;

    size_t offset = serdec_arena_alloc_offset(arena, sizeof(serdec_string_t), 
                                                _Alignof(serdec_string_t));
    serdec_string_t* s_str = serdec_arena_resolve(arena, offset);

    s_str->vec = serdec_vector_new(sizeof(char), len + 1, arena);
 
    char* buf = (char*) s_str->vec->data;
    if (len > 0) {
         memcpy(buf, str, len);
    }
    buf[len] = '\0';
    s_str->vec->count = len;

    return s_str;
}

void serdec_string_reserve(serdec_string_t* s_str, size_t len) {
    serdec_vector_reserve(s_str->vec, len);
}

void serdec_string_push(serdec_string_t* s_str, const char* str) {
    if (!str)
        return;
    
    size_t len = strlen(str);
    if (len == 0)
        return;
    
    size_t old_count = s_str->vec->count;
    serdec_vector_reserve(s_str->vec, old_count + len + 1);
    
    memcpy((char*) s_str->vec->data + old_count, str, len);
    ((char*)s_str->vec->data)[old_count + len] = '\0';
    s_str->vec->count += len;
}

void serdec_string_vprintf(serdec_string_t* s_str, const char* fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (needed < 0) {
        va_end(args_copy);
        return;
    }

    size_t start = s_str->vec->count;
    serdec_vector_reserve(s_str->vec, start + (size_t) needed + 1);

    int written = vsnprintf(((char*) s_str->vec->data) + start,
                            (size_t) needed + 1, fmt, args_copy);
    va_end(args_copy);

    if (written < 0)
        return;

    s_str->vec->count = start + (size_t) written;
}

void serdec_string_printf(serdec_string_t* s_str, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    serdec_string_vprintf(s_str, fmt, args);
    va_end(args);
}

size_t serdec_string_length(const serdec_string_t* s_str) {
    return s_str->vec->count;
}

void serdec_string_clear(serdec_string_t* s_str) {
    s_str->vec->count = 0;
}

char *serdec_string_take(serdec_string_t *s_str) {
    size_t len = s_str->vec->count;
    serdec_vector_reserve(s_str->vec, len + 1);
    char* buf = (char*) s_str->vec->data;
    buf[len] = '\0';
    char *out = buf;
    s_str->vec->data = NULL;
    s_str->vec->count = 0;
    s_str->vec->capacity = 0;
    return out;
}
