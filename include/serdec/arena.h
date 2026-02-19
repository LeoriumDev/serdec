#pragma once

#include <serdec/types.h>

/**
 * @brief Arena allocator configuration.
 */
typedef struct {
    size_t block_size;        /**< Block size in bytes. Default: 16KB. */
    size_t max_memory;        /**< Max total memory. Default: 256MB (DoS protection). */
    void* (*alloc)(size_t);   /**< Custom allocator. Default: malloc. */
    void (*free)(void*);      /**< Custom free. Default: free. */
} SerdecArenaConfig;

/**
 * @brief Create a new arena allocator.
 *
 * @param config Configuration, or NULL for defaults.
 * @return New arena, or NULL on allocation failure.
 */
SerdecArena* serdec_arena_create(const SerdecArenaConfig* config);

/**
 * @brief Destroy an arena and free all memory it owns.
 *
 * @param arena Arena to destroy.
 */
void serdec_arena_destroy(SerdecArena* arena);

/**
 * @brief Allocate bytes from the arena.
 *
 * @param arena Arena to allocate from.
 * @param size  Number of bytes to allocate.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void* serdec_arena_alloc(SerdecArena* arena, size_t size);

/**
 * @brief Allocate aligned bytes from the arena.
 *
 * @param arena Arena to allocate from.
 * @param size  Number of bytes to allocate.
 * @param align Alignment in bytes. Must be a power of 2.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void* serdec_arena_alloc_aligned(SerdecArena* arena, size_t size, size_t align);

/**
 * @brief Duplicate a string into the arena.
 *
 * @param arena Arena to allocate into.
 * @param str   Source string.
 * @param len   Length of source string in bytes.
 * @return Pointer to the duplicated string, or NULL on failure.
 */
char* serdec_arena_strdup(SerdecArena* arena, const char* str, size_t len);

/**
 * @brief Reset an arena, keeping the first block for reuse.
 *
 * @param arena Arena to reset.
 */
void serdec_arena_reset(SerdecArena* arena);

/**
 * @brief Return total bytes currently allocated in the arena.
 *
 * @param arena Arena to query.
 * @return Total bytes allocated.
 */
size_t serdec_arena_used(const SerdecArena* arena);
