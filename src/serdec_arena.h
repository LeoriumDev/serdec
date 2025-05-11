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

#pragma once

#include <stddef.h>

#if __STDC_VERSION__ < 202311L
#  define SERDEC_ALIGNOF(type) _Alignof(type)
#else
#  define SERDEC_ALIGNOF(type) alignof(type)
#endif

#define SERDEC_ARENA_ALLOC(arena, type) \
    serdec_arena_alloc(arena, sizeof(type), SERDEC_ALIGNOF(type))

typedef struct serdec_arena serdec_arena_t;

serdec_arena_t*  serdec_arena_create  (size_t           capacity);
void*            serdec_arena_alloc   (serdec_arena_t*  arena,
                                       size_t           size, 
                                       size_t           alignment);
void             serdec_arena_reset   (serdec_arena_t*  arena);
void             serdec_arena_destroy (serdec_arena_t*  arena);
