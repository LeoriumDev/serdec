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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "serdec_arena.h"
#include "serdec_utils.h"

struct serdec_arena {
    uint8_t* buffer;
    size_t   capacity;
    size_t   offset;
};

void serdec_arena_realloc(serdec_arena_t* arena, size_t required_size);

static inline uintptr_t serdec_align_up(uintptr_t value, size_t alignment) {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

serdec_arena_t* serdec_arena_create(size_t capacity) {
    if (!capacity)
        SERDEC_FATAL("serdec_arena_create: capacity cannot be zero");

    serdec_arena_t* arena = malloc(sizeof(serdec_arena_t));
    
    if (!arena)
        SERDEC_FATAL("serdec_arena_create: arena struct malloc failed");
    
    arena->buffer = malloc(capacity);
    if (!arena->buffer) {
        free(arena);
        SERDEC_FATAL("serdec_arena_create: arena buffer malloc failed");
    }

    arena->capacity = capacity;
    arena->offset = 0;

    return arena;
}

void* serdec_arena_alloc(serdec_arena_t* arena, size_t size, size_t alignment) {
    if (!arena)
        SERDEC_FATAL("serdec_arena_alloc: arena cannot be NULL");

    if (!size)
        SERDEC_FATAL("serdec_arena_alloc: cannot allocate 0 bytes");

    if (!alignment || (alignment & (alignment - 1)) != 0)
        SERDEC_FATAL("serdec_arena_alloc: alignment must be power of two");

    uintptr_t current = (uintptr_t) (arena->buffer + arena->offset);
    uintptr_t aligned = serdec_align_up(current, alignment);
    size_t padding = aligned - current;
    size_t total_size = padding + size;

    if (arena->offset + total_size > arena->capacity) {
        serdec_arena_realloc(arena, total_size);
        current = (uintptr_t) (arena->buffer + arena->offset);
        aligned = serdec_align_up(current, alignment);
        padding = aligned - current;
        total_size = padding + size;
    }

    void* ptr = (void*) aligned;
    arena->offset += total_size;
    return ptr;
}

void serdec_arena_realloc(serdec_arena_t* arena, size_t required_size) {
    if (!arena)
        SERDEC_FATAL("serdec_arena_realloc: arena cannot be NULL");

    size_t new_capacity = arena->capacity * 2;
    while (arena->offset + required_size > new_capacity)
        new_capacity *= 2;

    uint8_t* new_buffer = realloc(arena->buffer, new_capacity);
    if (!new_buffer)
        SERDEC_FATAL("serdec_arena_realloc: arena realloc failed");

    arena->buffer = new_buffer;
    arena->capacity = new_capacity;
}

void serdec_arena_reset(serdec_arena_t* arena) {
    if (!arena)
        SERDEC_FATAL("serdec_arena_reset: arena cannot be NULL");

    arena->offset = 0;
}

void serdec_arena_destroy(serdec_arena_t* arena) {
    if (!arena)
        SERDEC_FATAL("serdec_arena_destroy: arena cannot be NULL");

    free(arena->buffer);
    free(arena);
}
