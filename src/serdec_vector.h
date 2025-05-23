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

#ifndef SERDEC_VECTOR_H
#define SERDEC_VECTOR_H

#include "serdec_arena.h"

#define SERDEC_VECTOR_GROWTH_FACTOR 2
#define SERDEC_VECTOR_DEFAULT_CAPACITY 16

typedef struct serdec_vector {
    void*            data;
    size_t           count;
    size_t           capacity;
    size_t           elem_size;
    serdec_arena_t*  arena;
} serdec_vector_t;

serdec_vector_t*  serdec_vector_new      (size_t            elem_size,
                                          size_t            initial_capacity,
                                          serdec_arena_t*   arena);

void              serdec_vector_reserve  (serdec_vector_t*  vec,
                                          size_t            min_count);

void              serdec_vector_push     (serdec_vector_t*  vec,
                                          const void*       elem);

void              serdec_vector_pop      (serdec_vector_t*  vec,
                                          void*             out);

#endif /* SERDEC_VECTOR_H */
