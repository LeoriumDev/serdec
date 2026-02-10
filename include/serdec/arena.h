#pragma once

#include <serdec/types.h>

// Configuration
typedef struct {
    size_t block_size;        // Default: 16KB
    size_t max_memory;        // Default: 256MB (DoS protection)
    void* (*alloc)(size_t);   // Custom allocator (default: malloc)
    void (*free)(void*);      // Custom free (default: free)
} SerdecArenaConfig;

// Lifecycle
SerdecArena* serdec_arena_create(const SerdecArenaConfig* config);
void serdec_arena_destroy(SerdecArena* arena);

// Allocation
void* serdec_arena_alloc(SerdecArena* arena, size_t size);
void* serdec_arena_alloc_aligned(SerdecArena* arena, size_t size, size_t align);
char* serdec_arena_strdup(SerdecArena* arena, const char* str, size_t len);

// Management
void serdec_arena_reset(SerdecArena* arena);  // Reuse first block
size_t serdec_arena_used(const SerdecArena* arena);
