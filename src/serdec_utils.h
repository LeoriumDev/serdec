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

#include <stdio.h>
#include <stdlib.h>

#ifndef SERDEC_LOG_LEVEL
#define SERDEC_LOG_LEVEL 2
#endif

#define SERDEC_LOG_DEBUG 0
#define SERDEC_LOG_INFO  1
#define SERDEC_LOG_WARN  2
#define SERDEC_LOG_ERROR 3

#define SERDEC_LOG(level, msg)                             \
    do {                                                   \
        if (level >= SERDEC_LOG_LEVEL) {                   \
            fprintf(stderr, "[serdec] %s\n", msg);         \
            if (level == SERDEC_LOG_ERROR) abort();        \
        }                                                  \
    } while (0)

#define SERDEC_DEBUG(msg) SERDEC_LOG(SERDEC_LOG_DEBUG, msg)
#define SERDEC_INFO(msg)  SERDEC_LOG(SERDEC_LOG_INFO, msg)
#define SERDEC_WARN(msg)  SERDEC_LOG(SERDEC_LOG_WARN, msg)
#define SERDEC_FATAL(msg) SERDEC_LOG(SERDEC_LOG_ERROR, msg)
