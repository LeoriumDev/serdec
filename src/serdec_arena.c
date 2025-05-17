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
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "serdec_arena.h"

struct serdec_arena {
    uint8_t* buffer;
    size_t   capacity;
    size_t   offset;
};

bool serdec_arena_grow(serdec_arena_t* arena, size_t required_size);

static inline uintptr_t serdec_align_up(uintptr_t value, size_t alignment) {
    return (value + (alignment - 1)) & ~(alignment - 1);
}

serdec_arena_t* serdec_arena_create(size_t capacity) {
    if (!capacity)
        capacity = SERDEC_DEFAULT_CAPACITY;

    serdec_arena_t* arena = malloc(sizeof(serdec_arena_t));
    
    if (!arena)
        return NULL;
    
    arena->buffer = malloc(capacity);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }

    arena->capacity = capacity;
    arena->offset = 0;

    return arena;
}

size_t serdec_arena_alloc_offset(serdec_arena_t* arena, size_t size, size_t alignment) {
    if (!arena || !size || !alignment || (alignment & (alignment - 1)) != 0)
        return SERDEC_ARENA_INVALID_OFFSET;

    uintptr_t current = (uintptr_t) (arena->buffer + arena->offset);
    uintptr_t aligned = serdec_align_up(current, alignment);
    size_t padding = aligned - current;
    size_t total_size = padding + size;

    if (arena->offset + total_size > arena->capacity) {
        if (!serdec_arena_grow(arena, total_size))
            return SERDEC_ARENA_INVALID_OFFSET;

        current = (uintptr_t) (arena->buffer + arena->offset);
        aligned = serdec_align_up(current, alignment);
        padding = aligned - current;
        total_size = padding + size;
    }

    size_t result_offset = arena->offset + padding;
    arena->offset += total_size;
    
    return result_offset;
}

void* serdec_arena_resolve(serdec_arena_t* arena, size_t offset) {
    if (!arena || offset == SERDEC_ARENA_INVALID_OFFSET || offset > arena->capacity)
        return NULL;

    return (void*) (arena->buffer + offset);
}

bool serdec_arena_grow(serdec_arena_t* arena, size_t required_size) {
    if (!arena)
        return false;

    size_t new_capacity = arena->capacity * SERDEC_ARENA_GROWTH_FACTOR;
    if (arena->offset + required_size > new_capacity)
        new_capacity = arena->offset + required_size;

    uint8_t* new_buffer = realloc(arena->buffer, new_capacity);
    if (!new_buffer)
        return false;

    arena->buffer = new_buffer;
    arena->capacity = new_capacity;
    return true;
}

void serdec_arena_reset(serdec_arena_t* arena) {
    if (!arena)
        return;

    arena->offset = 0;
}

void serdec_arena_destroy(serdec_arena_t* arena) {
    if (!arena)
        return;

    free(arena->buffer);
    free(arena);
}
