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

#if __STDC_VERSION__ < 202311L
#include <stdalign.h>
#endif

#include "serdec_vector.h"
#include "serdec_utils.h"

#define SERDEC_VECTOR_GROWTH_FACTOR 2

serdec_vector_t* serdec_vector_new(size_t elem_size, size_t initial_capacity,
                                   serdec_arena_t* arena) {
    if (!elem_size)
        SERDEC_FATAL("serdec_vector_new: elem_size cannot be zero");

    if (!initial_capacity)
        SERDEC_FATAL("serdec_vector_new: initial_capacity cannot be zero");
    
    if (!arena)
        SERDEC_FATAL("serdec_vector_new: arena cannot be NULL");

    serdec_vector_t* vec = serdec_arena_alloc(arena, sizeof(*vec), 
                                    alignof(serdec_vector_t));

    vec->arena     = arena;
    vec->count     = 0;
    vec->capacity  = initial_capacity;
    vec->elem_size = elem_size;
    vec->data = NULL;
    serdec_vector_reserve(vec, initial_capacity);
    return vec;
}

void serdec_vector_reserve(serdec_vector_t* vec, size_t min_capacity) {
    if (!vec)
        SERDEC_FATAL("serdec_vector_reserve: vector cannot be NULL");

    if (!min_capacity)
        SERDEC_FATAL("serdec_vector_reserve: minimum capacity cannot be zero");

    if (vec->data != NULL && vec->capacity >= min_capacity)
         return;
    
    size_t new_cap = vec->capacity * SERDEC_VECTOR_GROWTH_FACTOR;
    if (new_cap < min_capacity)
        new_cap = min_capacity;

    void* new_data = serdec_arena_alloc(vec->arena,
                    new_cap * vec->elem_size, alignof(max_align_t));

    if (vec->data != NULL && vec->count > 0) {
        memcpy(new_data, vec->data, vec->count * vec->elem_size);
    }
    vec->data = new_data;
    vec->capacity = new_cap;
}

void serdec_vector_push(serdec_vector_t* vec, const void *elem) {
    if (!vec)
        SERDEC_FATAL("serdec_vector_push: vector cannot be NULL");
    if (!elem)
        SERDEC_FATAL("serdec_vector_push: element is NULL");

    if (vec->count == vec->capacity)
        serdec_vector_reserve(vec, vec->count + 1);

    memcpy(((char*) vec->data) + (vec->count * vec->elem_size), elem,
           vec->elem_size);
    vec->count++;
}

void serdec_vector_pop(serdec_vector_t* vec, void *out) {
    if (!vec)
        SERDEC_FATAL("serdec_vector_push: vector cannot be NULL");
    if (vec->count == 0)
        SERDEC_FATAL("serdec_vector_pop: vector is empty");
    vec->count--;
    if (out)
        memcpy(out, ((char*) vec->data) + (vec->count * vec->elem_size),
               vec->elem_size);
}
