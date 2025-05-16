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

#ifndef SERDEC_ARENA_H
#define SERDEC_ARENA_H

#include <stddef.h>

#define SERDEC_DEFAULT_CAPACITY 1024
#define SERDEC_INVALID_OFFSET ((size_t) - 1)
#define SERDEC_ARENA_GROWTH_FACTOR 2

#if __STDC_VERSION__ >= 202311L
    #define SERDEC_ALIGNOF(type) alignof(type)
#elif __STDC_VERSION__ >= 201112L
    #define SERDEC_ALIGNOF(type) _Alignof(type)
#else
    #if defined(__GNUC__) || defined(__clang__)
        #define SERDEC_ALIGNOF(type) __alignof__(type)
    #elif defined(_MSC_VER)
        #define SERDEC_ALIGNOF(type) __alignof(type)
    #else
        #define SERDEC_ALIGNOF(type) \
            ((sizeof(struct { char c; type m; }) - sizeof(type)))
    #endif
#endif

#define SERDEC_ARENA_ALLOC(arena, type) \
    serdec_arena_alloc(arena, sizeof(type), SERDEC_ALIGNOF(type))

typedef struct serdec_arena serdec_arena_t;

serdec_arena_t*  serdec_arena_create       (size_t           capacity);
size_t           serdec_arena_alloc_offset (serdec_arena_t*  arena,
                                            size_t           size, 
                                            size_t           alignment);
void*            serdec_arena_resolve      (serdec_arena_t* arena, size_t offset);
void             serdec_arena_reset        (serdec_arena_t*  arena);
void             serdec_arena_destroy      (serdec_arena_t*  arena);

#endif /* SERDEC_ARENA_H */
