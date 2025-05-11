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

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "serdec_arena.h"
#include "serdec_vector.h"

typedef struct serdec_string {
    serdec_vector_t* vec;
} serdec_string_t;

serdec_string_t*  serdec_string_new     (const char* str,
                                         serdec_arena_t* arena);

void              serdec_string_reserve (serdec_string_t* s,
                                         size_t len);

void              serdec_string_push    (serdec_string_t* s,
                                         const char* c);

void              serdec_string_vprintf (serdec_string_t* s,
                                         const char *fmt,
                                         va_list args);

void              serdec_string_printf  (serdec_string_t* s,
                                         const char *fmt,
                                         ...);

size_t            serdec_string_length  (const serdec_string_t *s);

void              serdec_string_clear   (serdec_string_t *s);

char*             serdec_string_take    (serdec_string_t *s);
