#include "test.h"
#include "../src/internal.h"
#include <serdec/serdec.h>

// === No escapes ===

TEST(unescape_plain) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "hello", 5, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 5);
    ASSERT(memcmp(out, "hello", 5) == 0);
    serdec_arena_destroy(arena);
}

TEST(unescape_empty) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "", 0, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 0);
    serdec_arena_destroy(arena);
}

// === Simple escapes ===

TEST(unescape_newline) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "a\\nb", 4, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 3);
    ASSERT_EQ(out[0], 'a');
    ASSERT_EQ(out[1], '\n');
    ASSERT_EQ(out[2], 'b');
    serdec_arena_destroy(arena);
}

TEST(unescape_all_simple) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \" \\ \/ \b \f \n \r \t
    ASSERT_EQ(serdec_string_unescape(arena, "\\\"\\\\\\b\\f\\n\\r\\t\\/", 16, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 8);
    ASSERT_EQ(out[0], '"');
    ASSERT_EQ(out[1], '\\');
    ASSERT_EQ(out[2], '\b');
    ASSERT_EQ(out[3], '\f');
    ASSERT_EQ(out[4], '\n');
    ASSERT_EQ(out[5], '\r');
    ASSERT_EQ(out[6], '\t');
    ASSERT_EQ(out[7], '/');
    serdec_arena_destroy(arena);
}

TEST(unescape_escaped_quote) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "say \\\"hi\\\"", 10, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 8);
    ASSERT(memcmp(out, "say \"hi\"", 8) == 0);
    serdec_arena_destroy(arena);
}

// === Unicode escapes ===

TEST(unescape_unicode_ascii) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \u0041 = 'A'
    ASSERT_EQ(serdec_string_unescape(arena, "\\u0041", 6, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 1);
    ASSERT_EQ(out[0], 'A');
    serdec_arena_destroy(arena);
}

TEST(unescape_unicode_2byte) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \u00E9 = é (2 UTF-8 bytes)
    ASSERT_EQ(serdec_string_unescape(arena, "\\u00e9", 6, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 2);
    ASSERT_EQ((unsigned char)out[0], 0xC3);
    ASSERT_EQ((unsigned char)out[1], 0xA9);
    serdec_arena_destroy(arena);
}

TEST(unescape_unicode_3byte) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \u4E2D = 中 (3 UTF-8 bytes)
    ASSERT_EQ(serdec_string_unescape(arena, "\\u4E2D", 6, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 3);
    ASSERT_EQ((unsigned char)out[0], 0xE4);
    ASSERT_EQ((unsigned char)out[1], 0xB8);
    ASSERT_EQ((unsigned char)out[2], 0xAD);
    serdec_arena_destroy(arena);
}

TEST(unescape_unicode_null) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \u0000 — valid, produces a null byte
    ASSERT_EQ(serdec_string_unescape(arena, "\\u0000", 6, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 1);
    ASSERT_EQ(out[0], '\0');
    serdec_arena_destroy(arena);
}

TEST(unescape_unicode_upper_lower_hex) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out1; char* out2; size_t len1, len2;
    // \u00E9 and \u00e9 should produce the same result
    ASSERT_EQ(serdec_string_unescape(arena, "\\u00E9", 6, &out1, &len1), SERDEC_OK);
    ASSERT_EQ(serdec_string_unescape(arena, "\\u00e9", 6, &out2, &len2), SERDEC_OK);
    ASSERT_EQ(len1, len2);
    ASSERT(memcmp(out1, out2, len1) == 0);
    serdec_arena_destroy(arena);
}

// === Surrogate pairs ===

TEST(unescape_surrogate_pair_emoji) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \uD83D\uDE00 = 😀 (U+1F600)
    ASSERT_EQ(serdec_string_unescape(arena, "\\uD83D\\uDE00", 12, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 4);
    ASSERT_EQ((unsigned char)out[0], 0xF0);
    ASSERT_EQ((unsigned char)out[1], 0x9F);
    ASSERT_EQ((unsigned char)out[2], 0x98);
    ASSERT_EQ((unsigned char)out[3], 0x80);
    serdec_arena_destroy(arena);
}

TEST(unescape_surrogate_pair_min) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \uD800\uDC00 = U+10000 (first valid surrogate pair)
    ASSERT_EQ(serdec_string_unescape(arena, "\\uD800\\uDC00", 12, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 4);
    // Verify roundtrip through decode
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode(out, len, &cp), 4);
    ASSERT_EQ(cp, 0x10000);
    serdec_arena_destroy(arena);
}

TEST(unescape_surrogate_pair_max) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \uDBFF\uDFFF = U+10FFFF (last valid surrogate pair)
    ASSERT_EQ(serdec_string_unescape(arena, "\\uDBFF\\uDFFF", 12, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 4);
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode(out, len, &cp), 4);
    ASSERT_EQ(cp, 0x10FFFF);
    serdec_arena_destroy(arena);
}

// === Invalid escapes ===

TEST(unescape_lone_high_surrogate) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "\\uD83D", 6, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_lone_low_surrogate) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "\\uDE00", 6, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_high_surrogate_no_pair) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // High surrogate followed by regular text, not \uXXXX
    ASSERT_EQ(serdec_string_unescape(arena, "\\uD83Dabc", 9, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_high_followed_by_non_surrogate) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // High surrogate followed by \u but not a low surrogate
    ASSERT_EQ(serdec_string_unescape(arena, "\\uD83D\\u0041", 12, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_bad_hex) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "\\uGHIJ", 6, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_truncated_unicode) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // Only 3 hex digits
    ASSERT_EQ(serdec_string_unescape(arena, "\\u123", 5, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_unknown_escape) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "\\a", 2, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_backslash_x) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // \x is not valid JSON escape
    ASSERT_EQ(serdec_string_unescape(arena, "\\x41", 4, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

TEST(unescape_trailing_backslash) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "abc\\", 4, &out, &len), SERDEC_ERR_INVALID_ESCAPE);
    serdec_arena_destroy(arena);
}

// === Mixed content ===

TEST(unescape_mixed) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // "line1\nline2"
    ASSERT_EQ(serdec_string_unescape(arena, "line1\\nline2", 12, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 11);
    ASSERT(memcmp(out, "line1\nline2", 11) == 0);
    serdec_arena_destroy(arena);
}

TEST(unescape_consecutive_escapes) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    ASSERT_EQ(serdec_string_unescape(arena, "\\n\\r\\t", 6, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 3);
    ASSERT_EQ(out[0], '\n');
    ASSERT_EQ(out[1], '\r');
    ASSERT_EQ(out[2], '\t');
    serdec_arena_destroy(arena);
}

TEST(unescape_unicode_in_middle) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // "caf\u00E9!" = "café!"
    ASSERT_EQ(serdec_string_unescape(arena, "caf\\u00E9!", 10, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 6);  // c a f [C3 A9] !
    ASSERT(memcmp(out, "caf\xC3\xA9!", 6) == 0);
    serdec_arena_destroy(arena);
}

TEST(unescape_emoji_in_text) {
    SerdecArena* arena = serdec_arena_create(NULL);
    char* out; size_t len;
    // "hi\uD83D\uDE00!" = "hi😀!"
    ASSERT_EQ(serdec_string_unescape(arena, "hi\\uD83D\\uDE00!", 15, &out, &len), SERDEC_OK);
    ASSERT_EQ(len, 7);  // h i [F0 9F 98 80] !
    ASSERT(memcmp(out, "hi\xF0\x9F\x98\x80!", 7) == 0);
    serdec_arena_destroy(arena);
}

// === Runner ===

int test_string(void) {
    printf("\n  String unescape tests:\n");

    // No escapes
    RUN(unescape_plain);
    RUN(unescape_empty);

    // Simple escapes
    RUN(unescape_newline);
    RUN(unescape_all_simple);
    RUN(unescape_escaped_quote);

    // Unicode escapes
    RUN(unescape_unicode_ascii);
    RUN(unescape_unicode_2byte);
    RUN(unescape_unicode_3byte);
    RUN(unescape_unicode_null);
    RUN(unescape_unicode_upper_lower_hex);

    // Surrogate pairs
    RUN(unescape_surrogate_pair_emoji);
    RUN(unescape_surrogate_pair_min);
    RUN(unescape_surrogate_pair_max);

    // Invalid escapes
    RUN(unescape_lone_high_surrogate);
    RUN(unescape_lone_low_surrogate);
    RUN(unescape_high_surrogate_no_pair);
    RUN(unescape_high_followed_by_non_surrogate);
    RUN(unescape_bad_hex);
    RUN(unescape_truncated_unicode);
    RUN(unescape_unknown_escape);
    RUN(unescape_backslash_x);
    RUN(unescape_trailing_backslash);

    // Mixed content
    RUN(unescape_mixed);
    RUN(unescape_consecutive_escapes);
    RUN(unescape_unicode_in_middle);
    RUN(unescape_emoji_in_text);

    TEST_SUMMARY();
}
