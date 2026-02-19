#include "test.h"
#include "../src/internal.h"
#include <serdec/serdec.h>

// === Decode: valid sequences ===

TEST(utf8_decode_ascii) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("A", 1, &cp), 1);
    ASSERT_EQ(cp, 'A');
}

TEST(utf8_decode_null_byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\x00", 1, &cp), 1);
    ASSERT_EQ(cp, 0);
}

TEST(utf8_decode_max_1byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\x7F", 1, &cp), 1);
    ASSERT_EQ(cp, 0x7F);
}

TEST(utf8_decode_min_2byte) {
    // U+0080 = 0xC2 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xC2\x80", 2, &cp), 2);
    ASSERT_EQ(cp, 0x80);
}

TEST(utf8_decode_2byte) {
    // U+00E9 (é) = 0xC3 0xA9
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xC3\xA9", 2, &cp), 2);
    ASSERT_EQ(cp, 0xE9);
}

TEST(utf8_decode_max_2byte) {
    // U+07FF = 0xDF 0xBF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xDF\xBF", 2, &cp), 2);
    ASSERT_EQ(cp, 0x7FF);
}

TEST(utf8_decode_min_3byte) {
    // U+0800 = 0xE0 0xA0 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xE0\xA0\x80", 3, &cp), 3);
    ASSERT_EQ(cp, 0x800);
}

TEST(utf8_decode_3byte) {
    // U+4E2D (中) = 0xE4 0xB8 0xAD
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xE4\xB8\xAD", 3, &cp), 3);
    ASSERT_EQ(cp, 0x4E2D);
}

TEST(utf8_decode_max_3byte) {
    // U+FFFF = 0xEF 0xBF 0xBF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xEF\xBF\xBF", 3, &cp), 3);
    ASSERT_EQ(cp, 0xFFFF);
}

TEST(utf8_decode_min_4byte) {
    // U+10000 = 0xF0 0x90 0x80 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF0\x90\x80\x80", 4, &cp), 4);
    ASSERT_EQ(cp, 0x10000);
}

TEST(utf8_decode_4byte_emoji) {
    // U+1F600 (😀) = 0xF0 0x9F 0x98 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF0\x9F\x98\x80", 4, &cp), 4);
    ASSERT_EQ(cp, 0x1F600);
}

TEST(utf8_decode_max_codepoint) {
    // U+10FFFF = 0xF4 0x8F 0xBF 0xBF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF4\x8F\xBF\xBF", 4, &cp), 4);
    ASSERT_EQ(cp, 0x10FFFF);
}

TEST(utf8_decode_only_first) {
    // Should decode only the first codepoint from a multi-char string
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("AB", 2, &cp), 1);
    ASSERT_EQ(cp, 'A');
}

// === Decode: invalid sequences ===

TEST(utf8_decode_empty) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("", 0, &cp), 0);
}

TEST(utf8_decode_overlong_2byte) {
    // U+0041 ('A') encoded as 2 bytes: 0xC1 0x81
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xC1\x81", 2, &cp), 0);
}

TEST(utf8_decode_overlong_3byte) {
    // U+007F encoded as 3 bytes: 0xE0 0x81 0xBF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xE0\x81\xBF", 3, &cp), 0);
}

TEST(utf8_decode_overlong_4byte) {
    // U+07FF encoded as 4 bytes: 0xF0 0x80 0x9F 0xBF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF0\x80\x9F\xBF", 4, &cp), 0);
}

TEST(utf8_decode_surrogate_high) {
    // U+D800 = 0xED 0xA0 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xED\xA0\x80", 3, &cp), 0);
}

TEST(utf8_decode_surrogate_low) {
    // U+DFFF = 0xED 0xBF 0xBF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xED\xBF\xBF", 3, &cp), 0);
}

TEST(utf8_decode_surrogate_mid) {
    // U+DB80 = 0xED 0xAE 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xED\xAE\x80", 3, &cp), 0);
}

TEST(utf8_decode_above_max) {
    // U+110000 = 0xF4 0x90 0x80 0x80
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF4\x90\x80\x80", 4, &cp), 0);
}

TEST(utf8_decode_truncated_2byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xC3", 1, &cp), 0);
}

TEST(utf8_decode_truncated_3byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xE4\xB8", 2, &cp), 0);
}

TEST(utf8_decode_truncated_4byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF0\x9F\x98", 3, &cp), 0);
}

TEST(utf8_decode_bare_continuation) {
    // 0x80 is a continuation byte, not a valid lead
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\x80", 1, &cp), 0);
}

TEST(utf8_decode_bad_continuation) {
    // 2-byte lead followed by non-continuation byte
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xC3\x00", 2, &cp), 0);
}

TEST(utf8_decode_invalid_lead_fe) {
    // 0xFE is never valid in UTF-8
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xFE", 1, &cp), 0);
}

TEST(utf8_decode_invalid_lead_ff) {
    // 0xFF is never valid in UTF-8
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xFF", 1, &cp), 0);
}

TEST(utf8_decode_invalid_lead_c0_c1) {
    // 0xC0 and 0xC1 always produce overlongs
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xC0\x80", 2, &cp), 0);
    ASSERT_EQ(serdec_utf8_decode("\xC1\xBF", 2, &cp), 0);
}

TEST(utf8_decode_invalid_lead_f5_plus) {
    // 0xF5+ would encode codepoints > U+10FFFF
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF5\x80\x80\x80", 4, &cp), 0);
}

TEST(utf8_decode_bad_continuation_3byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xE4\x00\xAD", 3, &cp), 0);
}

TEST(utf8_decode_bad_continuation_4byte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xF0\x9F\x00\x80", 4, &cp), 0);
}

TEST(utf8_decode_invalid_does_not_write_cp) {
    uint32_t cp = 0xDEADBEEF;
    ASSERT_EQ(serdec_utf8_decode("\xC3", 1, &cp), 0);
    ASSERT_EQ(cp, 0xDEADBEEF);
}

TEST(utf8_decode_null_outptr) {
    ASSERT_EQ(serdec_utf8_decode("A", 1, NULL), 0);
}

TEST(utf8_decode_only_first_multibyte) {
    uint32_t cp;
    ASSERT_EQ(serdec_utf8_decode("\xE4\xB8\xAD" "A", 4, &cp), 3);
    ASSERT_EQ(cp, 0x4E2D);
}

// === Encode ===

TEST(utf8_encode_ascii) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode('A', buf), 1);
    ASSERT_EQ(buf[0], 'A');
}

TEST(utf8_encode_null) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0, buf), 1);
    ASSERT_EQ(buf[0], '\0');
}

TEST(utf8_encode_2byte) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0xE9, buf), 2);
    ASSERT_EQ((unsigned char)buf[0], 0xC3);
    ASSERT_EQ((unsigned char)buf[1], 0xA9);
}

TEST(utf8_encode_3byte) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0x4E2D, buf), 3);
    ASSERT_EQ((unsigned char)buf[0], 0xE4);
    ASSERT_EQ((unsigned char)buf[1], 0xB8);
    ASSERT_EQ((unsigned char)buf[2], 0xAD);
}

TEST(utf8_encode_4byte) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0x1F600, buf), 4);
    ASSERT_EQ((unsigned char)buf[0], 0xF0);
    ASSERT_EQ((unsigned char)buf[1], 0x9F);
    ASSERT_EQ((unsigned char)buf[2], 0x98);
    ASSERT_EQ((unsigned char)buf[3], 0x80);
}

TEST(utf8_encode_surrogate_reject) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0xD800, buf), 0);
    ASSERT_EQ(serdec_utf8_encode(0xDFFF, buf), 0);
}

TEST(utf8_encode_above_max_reject) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0x110000, buf), 0);
}

TEST(utf8_encode_boundary_1_2) {
    char buf[4];
    // Last 1-byte
    ASSERT_EQ(serdec_utf8_encode(0x7F, buf), 1);
    // First 2-byte
    ASSERT_EQ(serdec_utf8_encode(0x80, buf), 2);
}

TEST(utf8_encode_boundary_2_3) {
    char buf[4];
    // Last 2-byte
    ASSERT_EQ(serdec_utf8_encode(0x7FF, buf), 2);
    // First 3-byte
    ASSERT_EQ(serdec_utf8_encode(0x800, buf), 3);
}

TEST(utf8_encode_boundary_3_4) {
    char buf[4];
    // Last 3-byte
    ASSERT_EQ(serdec_utf8_encode(0xFFFF, buf), 3);
    // First 4-byte
    ASSERT_EQ(serdec_utf8_encode(0x10000, buf), 4);
}

TEST(utf8_encode_does_not_overwrite_tail) {
    char buf[4] = { (char)0xAA, (char)0xAA, (char)0xAA, (char)0xAA };
    int n = serdec_utf8_encode(0x7F, buf);
    ASSERT_EQ(n, 1);
    ASSERT_EQ((unsigned char)buf[1], 0xAA);
    ASSERT_EQ((unsigned char)buf[2], 0xAA);
    ASSERT_EQ((unsigned char)buf[3], 0xAA);
}

TEST(utf8_encode_max_codepoint) {
    char buf[4];
    ASSERT_EQ(serdec_utf8_encode(0x10FFFF, buf), 4);
    ASSERT_EQ((unsigned char)buf[0], 0xF4);
    ASSERT_EQ((unsigned char)buf[1], 0x8F);
    ASSERT_EQ((unsigned char)buf[2], 0xBF);
    ASSERT_EQ((unsigned char)buf[3], 0xBF);
}

// === Roundtrip ===

TEST(utf8_roundtrip_all_widths) {
    uint32_t codepoints[] = { 0x00, 0x41, 0x7F, 0x80, 0xE9, 0x7FF,
                              0x800, 0x4E2D, 0xFFFD, 0xFFFF,
                              0x10000, 0x1F600, 0x10FFFF };
    for (int i = 0; i < 13; i++) {
        char buf[4];
        int enc_len = serdec_utf8_encode(codepoints[i], buf);
        ASSERT(enc_len > 0);

        uint32_t decoded;
        int dec_len = serdec_utf8_decode(buf, enc_len, &decoded);
        ASSERT_EQ(dec_len, enc_len);
        ASSERT_EQ(decoded, codepoints[i]);
    }
}

// === Validate ===

TEST(utf8_validate_ascii) {
    ASSERT(serdec_utf8_validate("Hello, world!", 13));
}

TEST(utf8_validate_empty) {
    ASSERT(serdec_utf8_validate("", 0));
}

TEST(utf8_validate_mixed) {
    // "café" = 63 61 66 C3 A9
    ASSERT(serdec_utf8_validate("caf\xC3\xA9", 5));
}

TEST(utf8_validate_emoji) {
    ASSERT(serdec_utf8_validate("\xF0\x9F\x98\x80", 4));
}

TEST(utf8_validate_multi_codepoint) {
    // A + é + 中 + 😀
    ASSERT(serdec_utf8_validate("A\xC3\xA9\xE4\xB8\xAD\xF0\x9F\x98\x80", 10));
}

TEST(utf8_validate_reject_overlong) {
    ASSERT(!serdec_utf8_validate("\xC1\x81", 2));
}

TEST(utf8_validate_reject_surrogate) {
    ASSERT(!serdec_utf8_validate("\xED\xA0\x80", 3));
}

TEST(utf8_validate_reject_truncated) {
    ASSERT(!serdec_utf8_validate("\xE4\xB8", 2));
}

TEST(utf8_validate_reject_bare_continuation) {
    ASSERT(!serdec_utf8_validate("\x80", 1));
}

TEST(utf8_validate_reject_bad_in_middle) {
    // Valid ASCII, then invalid byte, then more ASCII
    ASSERT(!serdec_utf8_validate("abc\xFE" "def", 7));
}

TEST(utf8_validate_reject_above_max) {
    ASSERT(!serdec_utf8_validate("\xF4\x90\x80\x80", 4));
}

TEST(utf8_validate_reject_invalid_lead_ranges) {
    ASSERT(!serdec_utf8_validate("\xC0\x80", 2));
    ASSERT(!serdec_utf8_validate("\xC1\xBF", 2));
    ASSERT(!serdec_utf8_validate("\xF5\x80\x80\x80", 4));
}

TEST(utf8_validate_allows_nul) {
    ASSERT(serdec_utf8_validate("A\0B", 3));
}

// === Runner ===

int test_utf8(void) {
    printf("\n  UTF-8 tests:\n");

    // Decode: valid
    RUN(utf8_decode_ascii);
    RUN(utf8_decode_null_byte);
    RUN(utf8_decode_max_1byte);
    RUN(utf8_decode_min_2byte);
    RUN(utf8_decode_2byte);
    RUN(utf8_decode_max_2byte);
    RUN(utf8_decode_min_3byte);
    RUN(utf8_decode_3byte);
    RUN(utf8_decode_max_3byte);
    RUN(utf8_decode_min_4byte);
    RUN(utf8_decode_4byte_emoji);
    RUN(utf8_decode_max_codepoint);
    RUN(utf8_decode_only_first);

    // Decode: invalid
    RUN(utf8_decode_empty);
    RUN(utf8_decode_overlong_2byte);
    RUN(utf8_decode_overlong_3byte);
    RUN(utf8_decode_overlong_4byte);
    RUN(utf8_decode_surrogate_high);
    RUN(utf8_decode_surrogate_low);
    RUN(utf8_decode_surrogate_mid);
    RUN(utf8_decode_above_max);
    RUN(utf8_decode_truncated_2byte);
    RUN(utf8_decode_truncated_3byte);
    RUN(utf8_decode_truncated_4byte);
    RUN(utf8_decode_bare_continuation);
    RUN(utf8_decode_bad_continuation);
    RUN(utf8_decode_bad_continuation_3byte);
    RUN(utf8_decode_bad_continuation_4byte);
    RUN(utf8_decode_invalid_lead_fe);
    RUN(utf8_decode_invalid_lead_ff);
    RUN(utf8_decode_invalid_lead_c0_c1);
    RUN(utf8_decode_invalid_lead_f5_plus);
    RUN(utf8_decode_invalid_does_not_write_cp);
    RUN(utf8_decode_null_outptr);
    RUN(utf8_decode_only_first_multibyte);

    // Encode
    RUN(utf8_encode_ascii);
    RUN(utf8_encode_null);
    RUN(utf8_encode_2byte);
    RUN(utf8_encode_3byte);
    RUN(utf8_encode_4byte);
    RUN(utf8_encode_surrogate_reject);
    RUN(utf8_encode_above_max_reject);
    RUN(utf8_encode_boundary_1_2);
    RUN(utf8_encode_boundary_2_3);
    RUN(utf8_encode_boundary_3_4);
    RUN(utf8_encode_does_not_overwrite_tail);
    RUN(utf8_encode_max_codepoint);

    // Roundtrip
    RUN(utf8_roundtrip_all_widths);

    // Validate
    RUN(utf8_validate_ascii);
    RUN(utf8_validate_empty);
    RUN(utf8_validate_mixed);
    RUN(utf8_validate_emoji);
    RUN(utf8_validate_multi_codepoint);
    RUN(utf8_validate_reject_overlong);
    RUN(utf8_validate_reject_surrogate);
    RUN(utf8_validate_reject_truncated);
    RUN(utf8_validate_reject_bare_continuation);
    RUN(utf8_validate_reject_bad_in_middle);
    RUN(utf8_validate_reject_above_max);
    RUN(utf8_validate_reject_invalid_lead_ranges);
    RUN(utf8_validate_allows_nul);

    TEST_SUMMARY();
}
