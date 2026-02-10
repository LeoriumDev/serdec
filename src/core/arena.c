#include "internal.h"

SerdecArena* serdec_arena_create(const SerdecArenaConfig* config) {
    return NULL;
}

void serdec_arena_destroy(SerdecArena* arena) {
    return;
}

void* serdec_arena_alloc(SerdecArena* arena, size_t size) {
    return NULL;
}

void* serdec_arena_alloc_aligned(SerdecArena* arena, size_t size, size_t align) {
    return NULL;
}

char* serdec_arena_strdup(SerdecArena* arena, const char* str, size_t len) {
    return NULL;
}

void serdec_arena_reset(SerdecArena* arena) {
    return;
}

size_t serdec_arena_used(const SerdecArena* arena) {
    return 0;
}