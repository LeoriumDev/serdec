#include "test.h"
#include <serdec/serdec.h>

TEST(buffer_create_from_string) {
    const char* json = "{\"test\": 123}";
    SerdecBuffer* buf = serdec_buffer_from_string(json, strlen(json));

    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(serdec_buffer_size(buf), strlen(json));
    ASSERT(memcmp(serdec_buffer_data(buf), json, strlen(json)) == 0);

    serdec_buffer_release(buf);
}

TEST(buffer_padding_is_zero) {
    const char* json = "test";  // 4 bytes
    SerdecBuffer* buf = serdec_buffer_from_string(json, 4);

    const char* data = serdec_buffer_data(buf);
    size_t size = serdec_buffer_size(buf);
    size_t cap = serdec_buffer_capacity(buf);

    for (size_t i = size; i < cap && i < size + 64; i++) {
        ASSERT_EQ(data[i], 0);
    }

    serdec_buffer_release(buf);
}

TEST(buffer_alignment) {
    SerdecBuffer* buf = serdec_buffer_from_string("x", 1);

    uintptr_t addr = (uintptr_t) serdec_buffer_data(buf);
    ASSERT((addr % 64) == 0);

    serdec_buffer_release(buf);
}

TEST(buffer_refcount) {
    SerdecBuffer* buf = serdec_buffer_from_string("test", 4);

    SerdecBuffer* ref1 = serdec_buffer_retain(buf);
    SerdecBuffer* ref2 = serdec_buffer_retain(buf);

    ASSERT(ref1 == buf);
    ASSERT(ref2 == buf);

    serdec_buffer_release(buf);
    serdec_buffer_release(ref1);
    serdec_buffer_release(ref2);
}

TEST(buffer_null_safety) {
    serdec_buffer_release(NULL);
    ASSERT_NULL(serdec_buffer_retain(NULL));
    ASSERT_NULL(serdec_buffer_data(NULL));
    ASSERT_EQ(serdec_buffer_size(NULL), SIZE_MAX);
}

TEST(buffer_empty_input) {
    SerdecBuffer* buf = serdec_buffer_from_string("", 0);

    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(serdec_buffer_size(buf), 0);
    ASSERT(serdec_buffer_capacity(buf) >= 64);

    serdec_buffer_release(buf);
}

TEST(buffer_null_string) {
    SerdecBuffer* buf = serdec_buffer_from_string(NULL, 10);
    ASSERT_NULL(buf);
}

TEST(buffer_large_input) {
    char large[1000];
    memset(large, 'x', sizeof(large));

    SerdecBuffer* buf = serdec_buffer_from_string(large, sizeof(large));
    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(serdec_buffer_size(buf), 1000);
    ASSERT(serdec_buffer_capacity(buf) >= 1000);

    serdec_buffer_release(buf);
}

TEST(buffer_binary_data) {
    const char data[] = {'{', '\0', '\0', '}'};
    SerdecBuffer* buf = serdec_buffer_from_string(data, 4);

    ASSERT_NOT_NULL(buf);
    ASSERT_EQ(serdec_buffer_size(buf), 4);
    ASSERT(memcmp(serdec_buffer_data(buf), data, 4) == 0);

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
    RUN(buffer_null_string);
    RUN(buffer_large_input);
    RUN(buffer_binary_data);

    TEST_SUMMARY();
}
