#include "internal.h"
#include "serdec/types.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define SERDEC_DEFAULT_BLOCK_SIZE  (16 * 1024)
#define SERDEC_DEFAULT_MAX_MEMORY  (256 * 1024 * 1024)

SerdecArena* serdec_arena_create(const SerdecArenaConfig* config) {
    size_t block_size = SERDEC_DEFAULT_BLOCK_SIZE;
    size_t max_memory = SERDEC_DEFAULT_MAX_MEMORY;
    void* (*mem_alloc)(size_t) = malloc;
    void (*mem_free)(void*) = free;

    if (config) {
        if (config->block_size)
            block_size = config->block_size;
        if (config->max_memory)
            max_memory = config->max_memory;
        if (config->alloc)
            mem_alloc = config->alloc;
        if (config->free)
            mem_free = config->free;
    }

    ArenaBlock* block = (ArenaBlock*) mem_alloc(sizeof(*block) + block_size);
    if (!block) return NULL; 

    *block = (ArenaBlock) {
        .next = NULL,
        .size = block_size,
        .used = 0,
    };
    
    SerdecArena* arena = (SerdecArena*) mem_alloc(sizeof(*arena));
    if (!arena) {
        mem_free(block);
        return NULL;
    }

    *arena = (SerdecArena) {
        .magic = SERDEC_MAGIC_ARENA,
        .current = block,
        .first = block,
        .total_allocated = block_size,
        .config = {
            .block_size = block_size,
            .max_memory = max_memory,
            .alloc = mem_alloc,
            .free = mem_free
        }
    };

    return arena;
}

void serdec_arena_destroy(SerdecArena* arena) {
    if (!arena || arena->magic != SERDEC_MAGIC_ARENA) return;

    arena->magic = SERDEC_MAGIC_FREED;

    ArenaBlock* next_block = arena->first;
    while (next_block) {
        ArenaBlock* current = next_block;
        next_block = next_block->next;
        arena->config.free(current);
    }
    
    arena->config.free(arena);
}

void* serdec_arena_alloc(SerdecArena* arena, size_t size) {
    if (!arena || !size || arena->magic != SERDEC_MAGIC_ARENA ||
        arena->total_allocated >= arena->config.max_memory) return NULL;
    
    // Large space allocation
    if (size > arena->config.block_size) {
        if (size > arena->config.max_memory - arena->total_allocated) return NULL;

        ArenaBlock* large_block = (ArenaBlock*) 
            arena->config.alloc(sizeof(*large_block) + size);
        if (!large_block) return NULL;

        *large_block = (ArenaBlock) {
            .next = arena->current->next,
            .size = size,
            .used = size,
        };
        
        arena->current->next = large_block;
        arena->total_allocated += size;
        return large_block->data;
    }

    // Normal space allocation
    if (size > arena->current->size - arena->current->used) {
        ArenaBlock* block = (ArenaBlock*) 
            arena->config.alloc(sizeof(*block) + arena->config.block_size);
        if (!block) return NULL;

        *block = (ArenaBlock) {
            .next = arena->current->next,
            .size = arena->config.block_size,
            .used = 0,
        };

        arena->current->next = block;
        arena->current = block;
        arena->total_allocated += arena->config.block_size;
    }

    void* new_space = arena->current->data + arena->current->used;
    arena->current->used += size;
    
    return new_space;
}

void* serdec_arena_alloc_aligned(SerdecArena* arena, size_t size, size_t align) {
    if (!arena || !size || !align ||
        arena->magic != SERDEC_MAGIC_ARENA) return NULL;
    
    uintptr_t aligned_ptr = (uintptr_t) (arena->current->data + arena->current->used);
    size_t padding = (align - (aligned_ptr % align)) % align;
    arena->current->used += padding;
    
    return serdec_arena_alloc(arena, size);
}

char* serdec_arena_strdup(SerdecArena* arena, const char* str, size_t len) {
    if (!str) return NULL;
    char* duplicated_str = (char*) serdec_arena_alloc(arena, len + 1);
    if (!duplicated_str) return NULL;

    memset(duplicated_str, 0, len + 1);
    memcpy(duplicated_str, str, len);

    return duplicated_str;
}

void serdec_arena_reset(SerdecArena* arena) {
    if (!arena || arena->magic != SERDEC_MAGIC_ARENA) return;

    ArenaBlock* head = arena->first;
    head = head->next; // Free all blocks except the first one
    while (head) {
        ArenaBlock* current = head;
        head = head->next;
        arena->config.free(current);
    }

    arena->current = arena->first;
    arena->first->next = NULL;
    arena->first->used = 0;
    arena->total_allocated = arena->first->size;
}

size_t serdec_arena_used(const SerdecArena* arena) {
    if (!arena || arena->magic != SERDEC_MAGIC_ARENA) return SIZE_MAX;

    size_t used_memory = 0;
    ArenaBlock* head = arena->first;
    while (head) {
        used_memory += head->used;
        head = head->next;
    }

    return used_memory;
}
