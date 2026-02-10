#include "test.h"
#include <serdec/serdec.h>

TEST(buffer_create_from_string) {
    const char *json = "{\"test\": 123}";
    SerdecBuffer *buf = serdec_buffer_from_string(json, strlen(json));

    assert(buf != NULL);
    assert(serdec_buffer_size(buf) == strlen(json));
    assert(memcmp(serdec_buffer_data(buf), json, strlen(json)) == 0);

    serdec_buffer_release(buf);
}

TEST(buffer_padding_is_zero) {
    const char *json = "test";  // 4 bytes
    SerdecBuffer *buf = serdec_buffer_from_string(json, 4);

    const char *data = serdec_buffer_data(buf);
    size_t size = serdec_buffer_size(buf);
    size_t cap = serdec_buffer_capacity(buf);

    // Check padding bytes are zero
    for (size_t i = size; i < cap && i < size + 64; i++) {
        assert(data[i] == 0);
    }

    serdec_buffer_release(buf);
}

TEST(buffer_alignment) {
    SerdecBuffer *buf = serdec_buffer_from_string("x", 1);

    uintptr_t addr = (uintptr_t)serdec_buffer_data(buf);
    assert((addr % 64) == 0);  // 64-byte aligned

    serdec_buffer_release(buf);
}

TEST(buffer_refcount) {
    SerdecBuffer *buf = serdec_buffer_from_string("test", 4);

    // Retain twice
    SerdecBuffer *ref1 = serdec_buffer_retain(buf);
    SerdecBuffer *ref2 = serdec_buffer_retain(buf);

    assert(ref1 == buf);
    assert(ref2 == buf);

    // Release all three references
    serdec_buffer_release(buf);
    serdec_buffer_release(ref1);
    serdec_buffer_release(ref2);

    // If we get here without ASan complaining, refcounting works
}

TEST(buffer_null_safety) {
    // These should not crash
    serdec_buffer_release(NULL);
    assert(serdec_buffer_retain(NULL) == NULL);
    assert(serdec_buffer_data(NULL) == NULL);
    assert(serdec_buffer_size(NULL) == 0);
}

TEST(buffer_empty_input) {
    SerdecBuffer *buf = serdec_buffer_from_string("", 0);

    assert(buf != NULL);
    assert(serdec_buffer_size(buf) == 0);
    assert(serdec_buffer_capacity(buf) >= 64);  // Still has padding

    serdec_buffer_release(buf);
}

int test_buffer(void) {
    printf("  Buffer tests:\n");

    RUN(buffer_create_from_string);
    RUN(buffer_padding_is_zero);
    RUN(buffer_alignment);
    RUN(buffer_refcount);
    RUN(buffer_null_safety);
    RUN(buffer_empty_input);

    printf("\n  All buffer tests passed!\n");

    return 0;
}