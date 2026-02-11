#include "test.h"
#include <serdec/serdec.h>

TEST(arena_create_destroy) {
    SerdecArenaConfig cfg = {
        .block_size = 1024,
        .max_memory = 1024 * 1024,
        .alloc = malloc,
        .free = free
    };

    SerdecArena* arena = serdec_arena_create(&cfg);
    ASSERT_NOT_NULL(arena);

    serdec_arena_destroy(arena);
}

TEST(arena_simple_alloc) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    int* x = serdec_arena_alloc(arena, sizeof(int));
    ASSERT_NOT_NULL(x);
    *x = 42;
    ASSERT_EQ(*x, 42);

    serdec_arena_destroy(arena);
}

TEST(arena_many_allocs) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    for (int i = 0; i < 10000; i++) {
        int* x = serdec_arena_alloc(arena, sizeof(int));
        ASSERT_NOT_NULL(x);
        *x = i;
    }

    serdec_arena_destroy(arena);
}

TEST(arena_memory_limit) {
    SerdecArenaConfig cfg = {
        .block_size = 1024,
        .max_memory = 4096
    };
    SerdecArena* arena = serdec_arena_create(&cfg);

    void* p1 = serdec_arena_alloc(arena, 1024);
    ASSERT_NOT_NULL(p1);

    void* p2 = serdec_arena_alloc(arena, 4096);
    ASSERT_NULL(p2);

    serdec_arena_destroy(arena);
}

TEST(arena_large_alloc) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    void* big = serdec_arena_alloc(arena, 8192);
    ASSERT_NOT_NULL(big);

    serdec_arena_destroy(arena);
}

TEST(arena_alignment) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    serdec_arena_alloc(arena, 3);

    void* aligned = serdec_arena_alloc_aligned(arena, 32, 16);
    ASSERT_NOT_NULL(aligned);
    ASSERT(((uintptr_t)aligned % 16) == 0);

    serdec_arena_destroy(arena);
}

TEST(arena_reset) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    for (int i = 0; i < 100; i++) {
        serdec_arena_alloc(arena, 100);
    }
    size_t used_before = serdec_arena_used(arena);
    ASSERT(used_before > 0);

    serdec_arena_reset(arena);

    void* p = serdec_arena_alloc(arena, 100);
    ASSERT_NOT_NULL(p);

    serdec_arena_destroy(arena);
}

TEST(arena_strdup) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    const char* original = "Hello, World!";
    char* copy = serdec_arena_strdup(arena, original, strlen(original));

    ASSERT_NOT_NULL(copy);
    ASSERT(copy != original);
    ASSERT(strcmp(copy, original) == 0);

    serdec_arena_destroy(arena);
}

TEST(arena_null_config) {
    SerdecArena* arena = serdec_arena_create(NULL);
    ASSERT_NOT_NULL(arena);

    void* p = serdec_arena_alloc(arena, 64);
    ASSERT_NOT_NULL(p);

    serdec_arena_destroy(arena);
}

TEST(arena_null_safety) {
    serdec_arena_destroy(NULL);
    ASSERT_NULL(serdec_arena_alloc(NULL, 10));
    ASSERT_NULL(serdec_arena_strdup(NULL, "hi", 2));
    serdec_arena_reset(NULL);
    ASSERT_EQ(serdec_arena_used(NULL), SIZE_MAX);
}

TEST(arena_zero_size_alloc) {
    SerdecArenaConfig cfg = { .block_size = 1024, .max_memory = 1024 * 1024 };
    SerdecArena* arena = serdec_arena_create(&cfg);

    void* p = serdec_arena_alloc(arena, 0);
    ASSERT_NULL(p);

    serdec_arena_destroy(arena);
}

int test_arena(void) {
    printf("\n  Arena tests:\n");

    RUN(arena_create_destroy);
    RUN(arena_simple_alloc);
    RUN(arena_many_allocs);
    RUN(arena_memory_limit);
    RUN(arena_large_alloc);
    RUN(arena_alignment);
    RUN(arena_reset);
    RUN(arena_strdup);
    RUN(arena_null_config);
    RUN(arena_null_safety);
    RUN(arena_zero_size_alloc);

    TEST_SUMMARY();
}
