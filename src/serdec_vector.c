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

#include <string.h>

#include "serdec_vector.h"
#include "serdec_arena.h"

serdec_vector_t* serdec_vector_new(size_t elem_size, size_t initial_capacity,
                                   serdec_arena_t* arena) {
    if (!elem_size || !arena)
        return NULL;

    if (!initial_capacity)
        initial_capacity = SERDEC_VECTOR_DEFAULT_CAPACITY;
    
    size_t vec_offset = serdec_arena_alloc_offset(arena, sizeof(serdec_vector_t), 
                                    alignof(serdec_vector_t));
    serdec_vector_t* vec = serdec_arena_resolve(arena, vec_offset);

    vec->arena     = arena;
    vec->count     = 0;
    vec->capacity  = initial_capacity;
    vec->elem_size = elem_size;
    vec->data = NULL;
    serdec_vector_reserve(vec, initial_capacity);
    return vec;
}

void serdec_vector_reserve(serdec_vector_t* vec, size_t min_capacity) {
    if (!vec || !min_capacity || vec->capacity >= min_capacity)
        return;
    
    size_t new_cap = vec->capacity * SERDEC_VECTOR_GROWTH_FACTOR;
    if (new_cap < min_capacity)
        new_cap = min_capacity;

    size_t offset = serdec_arena_alloc_offset(vec->arena,
                    new_cap * vec->elem_size, alignof(max_align_t));
    void* new_data = serdec_arena_resolve(vec->arena, offset);
    if (vec->data != NULL && vec->count > 0) {
        memcpy(new_data, vec->data, vec->count * vec->elem_size);
    }
    vec->data = new_data;
    vec->capacity = new_cap;
}

void serdec_vector_push(serdec_vector_t* vec, const void *elem) {
    if (!vec || !elem)
        return;

    if (vec->count == vec->capacity)
        serdec_vector_reserve(vec, vec->count + 1);

    memcpy(((char*) vec->data) + (vec->count * vec->elem_size), elem,
           vec->elem_size);
    vec->count++;
}

void serdec_vector_pop(serdec_vector_t* vec, void *out) {
    if (!vec || vec->count == 0)
        return;

    vec->count--;
    if (out)
        memcpy(out, ((char*) vec->data) + (vec->count * vec->elem_size),
               vec->elem_size);
}
