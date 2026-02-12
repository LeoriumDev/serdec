#include "test.h"
#include "../src/internal.h"
#include <serdec/serdec.h>
#include <math.h>
#include <locale.h>

static SerdecLexer* make_lexer(const char* json) {
    SerdecBuffer* buf = serdec_buffer_from_string(json, strlen(json));
    SerdecLexer* lex = serdec_lexer_create(buf);
    serdec_buffer_release(buf); // lexer retains it
    return lex;
}

// --- Structural tokens ---

TEST(lex_structural) {
    SerdecLexer* lex = make_lexer("{}[],:");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COLON);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_structural_start_length) {
    SerdecLexer* lex = make_lexer("{");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(tok.length, 1);
    ASSERT_EQ(*tok.start, '{');
    serdec_lexer_destroy(lex);
}

// --- Whitespace ---

TEST(lex_whitespace) {
    SerdecLexer* lex = make_lexer("  \t\n\r  true  ");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_only_whitespace) {
    SerdecLexer* lex = make_lexer("   \n\t\r  ");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_empty_input) {
    SerdecLexer* lex = make_lexer("");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Keywords ---

TEST(lex_keywords) {
    SerdecLexer* lex = make_lexer("true false null");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_FALSE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NULL);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_keyword_prefix_reject) {
    SerdecLexer* lex = make_lexer("nullify");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);

    lex = make_lexer("trueish");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);

    lex = make_lexer("falsehood");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);
}

TEST(lex_keyword_case_sensitive) {
    // 'T', 'N', 'F' don't match 't', 'n', 'f' in the dispatcher —
    // they hit default case, so error is UNEXPECTED_CHAR, not INVALID_VALUE
    SerdecLexer* lex = make_lexer("True");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);

    lex = make_lexer("NULL");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);

    lex = make_lexer("FALSE");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

TEST(lex_keyword_at_end) {
    SerdecLexer* lex = make_lexer("true");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(tok.length, 4);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Strings ---

TEST(lex_string_simple) {
    SerdecLexer* lex = make_lexer("\"hello\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(tok.length, 5);
    ASSERT(memcmp(tok.start, "hello", 5) == 0);
    ASSERT(!tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_empty) {
    SerdecLexer* lex = make_lexer("\"\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(tok.length, 0);
    ASSERT(!tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_with_escapes) {
    SerdecLexer* lex = make_lexer("\"hello\\nworld\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_escaped_quote) {
    SerdecLexer* lex = make_lexer("\"say \\\"hi\\\"\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    // Should be followed by EOF, not another string
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_escaped_backslash) {
    SerdecLexer* lex = make_lexer("\"a\\\\b\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_unterminated) {
    SerdecLexer* lex = make_lexer("\"hello");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNTERMINATED_STRING);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_control_char) {
    // Raw tab inside string is invalid
    SerdecLexer* lex = make_lexer("\"hello\tworld\"");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_newline_reject) {
    // Raw newline inside string is invalid
    SerdecLexer* lex = make_lexer("\"hello\nworld\"");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_null_byte) {
    // Null byte (0x00) is a control character, should be rejected
    SerdecBuffer* buf = serdec_buffer_from_string("\"ab\0cd\"", 7);
    SerdecLexer* lex = serdec_lexer_create(buf);
    serdec_buffer_release(buf);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

// --- Numbers: integers ---

TEST(lex_number_zero) {
    SerdecLexer* lex = make_lexer("0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT(!tok.number.is_negative);
    ASSERT_EQ(tok.number.value.u64, 0);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_simple_int) {
    SerdecLexer* lex = make_lexer("42");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT_EQ(tok.number.value.u64, 42);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_negative_int) {
    SerdecLexer* lex = make_lexer("-123");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT(tok.number.is_negative);
    ASSERT_EQ(tok.number.value.i64, -123);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_negative_zero) {
    SerdecLexer* lex = make_lexer("-0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_large_int) {
    SerdecLexer* lex = make_lexer("9223372036854775807"); // INT64_MAX
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT_EQ(tok.number.value.u64, 9223372036854775807ULL);
    serdec_lexer_destroy(lex);
}

// --- Numbers: floats ---

TEST(lex_number_simple_float) {
    SerdecLexer* lex = make_lexer("3.14");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 3.14) < 0.001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_negative_float) {
    SerdecLexer* lex = make_lexer("-2.5");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(tok.number.is_negative);
    ASSERT(fabs(tok.number.value.f64 - (-2.5)) < 0.001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_zero_point) {
    SerdecLexer* lex = make_lexer("0.0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64) < 0.001);
    serdec_lexer_destroy(lex);
}

// --- Numbers: scientific notation ---

TEST(lex_number_exponent) {
    SerdecLexer* lex = make_lexer("1e10");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 1e10) < 1.0);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_exponent_upper) {
    SerdecLexer* lex = make_lexer("1E10");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 1e10) < 1.0);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_exponent_positive) {
    SerdecLexer* lex = make_lexer("1e+10");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_exponent_negative) {
    SerdecLexer* lex = make_lexer("1.23e-5");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 1.23e-5) < 1e-10);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_fraction_and_exponent) {
    SerdecLexer* lex = make_lexer("1.5e2");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 150.0) < 0.001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_large_exponent) {
    SerdecLexer* lex = make_lexer("1e308");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_small_exponent) {
    SerdecLexer* lex = make_lexer("1e-400");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);
}

// --- Numbers: invalid ---

TEST(lex_number_leading_zero) {
    SerdecLexer* lex = make_lexer("01");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_leading_zero_neg) {
    SerdecLexer* lex = make_lexer("-01");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_trailing_dot) {
    SerdecLexer* lex = make_lexer("1.");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_leading_dot) {
    // ".5" — '.' hits default case in dispatcher → UNEXPECTED_CHAR
    SerdecLexer* lex = make_lexer(".5");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_missing_exp_digits) {
    SerdecLexer* lex = make_lexer("1e");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_explicit_plus) {
    // "+5" — '+' hits default case in dispatcher → UNEXPECTED_CHAR
    SerdecLexer* lex = make_lexer("+5");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_double_minus) {
    SerdecLexer* lex = make_lexer("--5");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_minus_alone) {
    SerdecLexer* lex = make_lexer("-");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_multiple_dots) {
    SerdecLexer* lex = make_lexer("1.2.3");
    SerdecToken tok = serdec_lexer_next(lex);
    // Should parse "1.2" then "." is unexpected
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(fabs(tok.number.value.f64 - 1.2) < 0.001);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_exp_no_sign_digits) {
    SerdecLexer* lex = make_lexer("1e+");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

// --- Numbers: in context ---

TEST(lex_number_followed_by_comma) {
    SerdecLexer* lex = make_lexer("123,456");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 123);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 456);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_followed_by_brace) {
    SerdecLexer* lex = make_lexer("42}");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 42);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACE);
    serdec_lexer_destroy(lex);
}

// --- Peek ---

TEST(lex_peek) {
    SerdecLexer* lex = make_lexer("[1, 2]");
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_peek_at_eof) {
    SerdecLexer* lex = make_lexer("");
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_EOF);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Full JSON tokenization ---

TEST(lex_full_object) {
    SerdecLexer* lex = make_lexer("{\"name\": \"Alice\", \"age\": 30}");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COLON);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COLON);
    SerdecToken num = serdec_lexer_next(lex);
    ASSERT_EQ(num.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(num.number.value.u64, 30);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_full_array) {
    SerdecLexer* lex = make_lexer("[1, true, \"hi\", null]");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NULL);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Unexpected characters ---

TEST(lex_unexpected_char) {
    SerdecLexer* lex = make_lexer("@");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

TEST(lex_null_safety) {
    ASSERT_EQ(serdec_lexer_next(NULL).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_peek(NULL).type, SERDEC_TOKEN_ERROR);
    ASSERT_NULL(serdec_lexer_get_error(NULL));
    serdec_lexer_destroy(NULL);
}

// --- Keywords: truncated ---

TEST(lex_keyword_truncated) {
    // "tru" is not "true"
    SerdecLexer* lex = make_lexer("tru");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);

    lex = make_lexer("fals");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);

    lex = make_lexer("nul");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);

    // Single first letter
    lex = make_lexer("t");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);
}

TEST(lex_keyword_followed_by_structural) {
    // "true," should parse as TRUE then COMMA
    SerdecLexer* lex = make_lexer("true,false]null}");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_FALSE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NULL);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_keyword_with_digit_suffix) {
    // "true0" should fail (digit is alnum)
    SerdecLexer* lex = make_lexer("true0");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);

    lex = make_lexer("null9");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);
}

// --- Strings: more edge cases ---

TEST(lex_string_all_escape_types) {
    // All valid JSON escape sequences: \" \\ \/ \b \f \n \r \t
    SerdecLexer* lex = make_lexer("\"\\\"\\\\\\/ \\b\\f\\n\\r\\t\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_unicode_escape) {
    // \u0041 is 'A'
    SerdecLexer* lex = make_lexer("\"\\u0041\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_backslash_at_end) {
    // String ends with backslash — unterminated escape
    SerdecLexer* lex = make_lexer("\"abc\\");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_only_escapes) {
    SerdecLexer* lex = make_lexer("\"\\n\\t\\r\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    // Raw length should be 6: \n \t \r (each is 2 chars in source)
    ASSERT_EQ(tok.length, 6);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_escaped_backslash_before_quote) {
    // "\\" — escaped backslash, then closing quote
    // The string content is just one backslash
    SerdecLexer* lex = make_lexer("\"\\\\\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    ASSERT_EQ(tok.length, 2); // two chars: backslash backslash
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_multiple_sequential) {
    SerdecLexer* lex = make_lexer("\"a\" \"b\" \"c\"");
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    }
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_just_quote) {
    // Single double-quote — unterminated
    SerdecLexer* lex = make_lexer("\"");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNTERMINATED_STRING);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_carriage_return) {
    // Raw \r inside string is control char (0x0D < 0x20)
    SerdecLexer* lex = make_lexer("\"hello\rworld\"");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

// --- Numbers: integer boundaries ---

TEST(lex_number_uint64_max) {
    SerdecLexer* lex = make_lexer("18446744073709551615"); // UINT64_MAX
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT(!tok.number.is_negative);
    ASSERT(tok.number.value.u64 == 18446744073709551615ULL);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_uint64_overflow) {
    // UINT64_MAX + 1 = 18446744073709551616
    SerdecLexer* lex = make_lexer("18446744073709551616");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_NUMBER_OVERFLOW);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_int64_min) {
    // INT64_MIN = -9223372036854775808
    SerdecLexer* lex = make_lexer("-9223372036854775808");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT(tok.number.is_negative);
    // INT64_MIN is -(2^63), check via cast
    ASSERT(tok.number.value.i64 == (int64_t)(-9223372036854775807LL - 1));
    serdec_lexer_destroy(lex);
}

TEST(lex_number_int64_overflow) {
    // One beyond INT64_MIN: -9223372036854775809
    SerdecLexer* lex = make_lexer("-9223372036854775809");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_NUMBER_OVERFLOW);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_single_digit_each) {
    const char* digits[] = {"0","1","2","3","4","5","6","7","8","9"};
    for (int i = 0; i < 10; i++) {
        SerdecLexer* lex = make_lexer(digits[i]);
        SerdecToken tok = serdec_lexer_next(lex);
        ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
        ASSERT(tok.number.is_integer);
        ASSERT_EQ(tok.number.value.u64, (uint64_t)i);
        serdec_lexer_destroy(lex);
    }
}

// --- Numbers: more float edge cases ---

TEST(lex_number_zero_exponent) {
    // 0e0 = 0.0
    SerdecLexer* lex = make_lexer("0e0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64) < 0.001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_negative_zero_float) {
    SerdecLexer* lex = make_lexer("-0.0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(tok.number.is_negative);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_negative_with_exponent) {
    SerdecLexer* lex = make_lexer("-1e5");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(tok.number.is_negative);
    ASSERT(fabs(tok.number.value.f64 - (-1e5)) < 1.0);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_many_fraction_digits) {
    SerdecLexer* lex = make_lexer("3.141592653589793");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 3.141592653589793) < 1e-9);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_exponent_e0) {
    // 123e0 = 123.0
    SerdecLexer* lex = make_lexer("123e0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 123.0) < 0.001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_frac_exponent_negative) {
    // 0.1e1 = 1.0
    SerdecLexer* lex = make_lexer("0.1e1");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 1.0) < 0.001);
    serdec_lexer_destroy(lex);
}

// --- Numbers: more invalid ---

TEST(lex_number_hex_reject) {
    SerdecLexer* lex = make_lexer("0x1F");
    // "0" is valid, then "x" is unexpected next call
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 0);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR); // 'x'
    serdec_lexer_destroy(lex);
}

TEST(lex_number_octal_reject) {
    // "00" — leading zero followed by digit
    SerdecLexer* lex = make_lexer("00");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_dot_exp) {
    // "1.e5" — dot with no fraction digits
    SerdecLexer* lex = make_lexer("1.e5");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_double_exp) {
    // "1e2e3" — parse "1e2" then "e" is unexpected
    SerdecLexer* lex = make_lexer("1e2e3");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR); // 'e'
    serdec_lexer_destroy(lex);
}

TEST(lex_number_minus_dot) {
    // "-." — minus with no digits
    SerdecLexer* lex = make_lexer("-.");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_minus_e) {
    // "-e5" — minus with no digits before exponent
    SerdecLexer* lex = make_lexer("-e5");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_huge_exponent) {
    // Exponent so large it would overflow int64, should clamp or handle gracefully
    SerdecLexer* lex = make_lexer("1e99999999999999999");
    SerdecToken tok = serdec_lexer_next(lex);
    // Should still parse without crashing (may be inf)
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);
}

// --- Numbers: context and boundaries ---

TEST(lex_number_followed_by_bracket) {
    SerdecLexer* lex = make_lexer("99]");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 99);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_followed_by_colon) {
    // Weird but lexer doesn't care about grammar
    SerdecLexer* lex = make_lexer("1:2");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 1);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COLON);
    tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 2);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_followed_by_whitespace) {
    SerdecLexer* lex = make_lexer("  42  ");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 42);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Peek: more patterns ---

TEST(lex_peek_preserves_value) {
    SerdecLexer* lex = make_lexer("42");
    SerdecToken peeked = serdec_lexer_peek(lex);
    ASSERT_EQ(peeked.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(peeked.number.value.u64, 42);
    SerdecToken consumed = serdec_lexer_next(lex);
    ASSERT_EQ(consumed.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(consumed.number.value.u64, 42);
    serdec_lexer_destroy(lex);
}

TEST(lex_peek_next_alternating) {
    SerdecLexer* lex = make_lexer("[1,2,3]");
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_eof_repeatable) {
    // Calling next() past EOF should keep returning EOF
    SerdecLexer* lex = make_lexer("1");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Unexpected characters: variety ---

TEST(lex_unexpected_chars_variety) {
    const char* bad_inputs[] = {"@", "#", "$", "%", "&", "!", "~", "`", "^", "'", ";"};
    for (int i = 0; i < 11; i++) {
        SerdecLexer* lex = make_lexer(bad_inputs[i]);
        ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
        serdec_lexer_destroy(lex);
    }
}

// --- Full JSON: complex ---

TEST(lex_nested_json) {
    SerdecLexer* lex = make_lexer("{\"a\":{\"b\":[1,2,{\"c\":true}]}}");
    // { "a" : { "b" : [ 1 , 2 , { "c" : true } ] } }
    SerdecTokenType expected[] = {
        SERDEC_TOKEN_LBRACE,    // {
        SERDEC_TOKEN_STRING,    // "a"
        SERDEC_TOKEN_COLON,     // :
        SERDEC_TOKEN_LBRACE,    // {
        SERDEC_TOKEN_STRING,    // "b"
        SERDEC_TOKEN_COLON,     // :
        SERDEC_TOKEN_LBRACKET,  // [
        SERDEC_TOKEN_NUMBER,    // 1
        SERDEC_TOKEN_COMMA,     // ,
        SERDEC_TOKEN_NUMBER,    // 2
        SERDEC_TOKEN_COMMA,     // ,
        SERDEC_TOKEN_LBRACE,    // {
        SERDEC_TOKEN_STRING,    // "c"
        SERDEC_TOKEN_COLON,     // :
        SERDEC_TOKEN_TRUE,      // true
        SERDEC_TOKEN_RBRACE,    // }
        SERDEC_TOKEN_RBRACKET,  // ]
        SERDEC_TOKEN_RBRACE,    // }
        SERDEC_TOKEN_RBRACE,    // }
        SERDEC_TOKEN_EOF,
    };
    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(serdec_lexer_next(lex).type, expected[i]);
    }
    serdec_lexer_destroy(lex);
}

TEST(lex_all_value_types) {
    // Array with every JSON value type
    SerdecLexer* lex = make_lexer("[\"str\", 42, -1.5e2, true, false, null]");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);

    SerdecToken sci = serdec_lexer_next(lex);
    ASSERT_EQ(sci.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!sci.number.is_integer);
    ASSERT(sci.number.is_negative);
    ASSERT(fabs(sci.number.value.f64 - (-150.0)) < 0.001);

    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_FALSE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COMMA);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NULL);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- Line/column tracking ---

TEST(lex_error_position_line_column) {
    // "{\n  @" — error at line 2, column 3
    SerdecLexer* lex = make_lexer("{\n  @");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_EQ(err->line, 2);
    ASSERT_EQ(err->column, 3);
    serdec_lexer_destroy(lex);
}

TEST(lex_error_position_offset) {
    // "@" at offset 0
    SerdecLexer* lex = make_lexer("@");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_EQ(err->offset, 0);
    serdec_lexer_destroy(lex);
}

// --- Create/destroy edge cases ---

TEST(lex_create_null_buffer) {
    ASSERT_NULL(serdec_lexer_create(NULL));
}

// --- 1. Token span + length invariants ---

TEST(lex_structural_all_length_one) {
    const char* inputs[] = {"{", "}", "[", "]", ",", ":"};
    for (int i = 0; i < 6; i++) {
        SerdecLexer* lex = make_lexer(inputs[i]);
        SerdecToken tok = serdec_lexer_next(lex);
        ASSERT_EQ(tok.length, 1);
        ASSERT_EQ(*tok.start, inputs[i][0]);
        serdec_lexer_destroy(lex);
    }
}

TEST(lex_keyword_span) {
    SerdecLexer* lex = make_lexer("true false null");
    SerdecToken t = serdec_lexer_next(lex);
    ASSERT_EQ(t.length, 4);
    ASSERT(memcmp(t.start, "true", 4) == 0);
    t = serdec_lexer_next(lex);
    ASSERT_EQ(t.length, 5);
    ASSERT(memcmp(t.start, "false", 5) == 0);
    t = serdec_lexer_next(lex);
    ASSERT_EQ(t.length, 4);
    ASSERT(memcmp(t.start, "null", 4) == 0);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_token_length) {
    // token.length must exactly match source lexeme length
    struct { const char* input; size_t len; } cases[] = {
        {"0", 1}, {"-0", 2}, {"42", 2}, {"-123", 4},
        {"3.14", 4}, {"0e0", 3}, {"1e+10", 5}, {"1.2300", 6},
        {"-1.5e-3", 7},
    };
    for (int i = 0; i < 9; i++) {
        SerdecLexer* lex = make_lexer(cases[i].input);
        SerdecToken tok = serdec_lexer_next(lex);
        ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
        ASSERT_EQ(tok.length, cases[i].len);
        serdec_lexer_destroy(lex);
    }
}

// --- 2. Peek/next identity (pointer + value, not just type) ---

TEST(lex_peek_pointer_identity) {
    SerdecLexer* lex = make_lexer("\"hello\"");
    SerdecToken p1 = serdec_lexer_peek(lex);
    SerdecToken p2 = serdec_lexer_peek(lex);
    // Both peeks must return identical token
    ASSERT_EQ(p1.type, p2.type);
    ASSERT(p1.start == p2.start);
    ASSERT_EQ(p1.length, p2.length);
    // next() must return the same token as peek
    SerdecToken n = serdec_lexer_next(lex);
    ASSERT_EQ(n.type, p1.type);
    ASSERT(n.start == p1.start);
    ASSERT_EQ(n.length, p1.length);
    serdec_lexer_destroy(lex);
}

TEST(lex_peek_eof_identity) {
    SerdecLexer* lex = make_lexer("");
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_EOF);
    ASSERT_EQ(serdec_lexer_peek(lex).type, SERDEC_TOKEN_EOF);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- 3. Error state contract ---

TEST(lex_error_sticky_unexpected) {
    // After unexpected char, lexer is stuck (current not advanced past bad byte).
    // Calling next() repeatedly should keep returning ERROR.
    SerdecLexer* lex = make_lexer("@");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    serdec_lexer_destroy(lex);
}

TEST(lex_error_info_populated) {
    SerdecLexer* lex = make_lexer("@");
    serdec_lexer_next(lex);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_NOT_NULL(err);
    ASSERT_EQ(err->code, SERDEC_ERR_UNEXPECTED_CHAR);
    ASSERT_EQ(err->line, 1);
    ASSERT_EQ(err->column, 1);
    serdec_lexer_destroy(lex);
}

TEST(lex_error_info_stable) {
    // get_error returns same pointer each time
    SerdecLexer* lex = make_lexer("@");
    serdec_lexer_next(lex);
    const SerdecErrorInfo* e1 = serdec_lexer_get_error(lex);
    const SerdecErrorInfo* e2 = serdec_lexer_get_error(lex);
    ASSERT(e1 == e2);
    ASSERT_EQ(e1->code, e2->code);
    serdec_lexer_destroy(lex);
}

// --- 5. String escape edge cases ---
// NOTE: The lexer does NOT validate individual escape characters.
// It just skips past \\ + next char and sets has_escapes = true.
// Escape validation (\q rejection, \uXXXX hex check, surrogate pairs)
// belongs to the string decoder layer (not implemented yet).

TEST(lex_string_escape_validation_deferred) {
    // "\q" is accepted by the lexer — escape validation is deferred to decoder
    SerdecLexer* lex = make_lexer("\"\\q\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_escaped_slash) {
    // \/ is valid in JSON (escaped forward slash)
    SerdecLexer* lex = make_lexer("\"\\/\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

// --- 6. Number grammar edge cases ---

TEST(lex_number_exponent_leading_zero) {
    // Exponent CAN have leading zeros (unlike integer part per RFC 8259)
    SerdecLexer* lex = make_lexer("1e01");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 10.0) < 0.001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_zero_exp_variants) {
    SerdecLexer* lex = make_lexer("0E-1");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);

    lex = make_lexer("0e+1");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_trailing_zeros_valid) {
    // Trailing zeros in fraction are valid; token length includes them
    SerdecLexer* lex = make_lexer("1.2300");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.length, 6);
    ASSERT(fabs(tok.number.value.f64 - 1.23) < 0.0001);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_double_sign_exp) {
    SerdecLexer* lex = make_lexer("1e--1");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);

    lex = make_lexer("1e+-1");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_float_infinity) {
    // 1e309 overflows double to infinity; should parse without crashing
    SerdecLexer* lex = make_lexer("1e309");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    serdec_lexer_destroy(lex);

    lex = make_lexer("-1e309");
    tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_negative);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_negative_zero_contract) {
    // Lock down -0 behavior: integer, negative, value 0
    SerdecLexer* lex = make_lexer("-0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer);
    ASSERT(tok.number.is_negative);
    ASSERT_EQ(tok.number.value.i64, 0);
    serdec_lexer_destroy(lex);
}

// --- 7. Lexer doesn't validate JSON grammar ---

TEST(lex_grammar_agnostic) {
    // "]" alone — lexer doesn't care about nesting
    SerdecLexer* lex = make_lexer("]");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACKET);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);

    // "{:}" — colon without key/value is grammar error, but lexer is fine
    lex = make_lexer("{:}");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_COLON);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_RBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- 8. Mixed-token adjacency (no whitespace) ---

TEST(lex_adjacent_keyword_keyword) {
    // "truefalse" — keyword boundary check rejects (is_alnum('f') = true)
    SerdecLexer* lex = make_lexer("truefalse");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_VALUE);
    serdec_lexer_destroy(lex);
}

TEST(lex_zero_then_keyword) {
    // "0true" — number stops at 't', then "true" is a separate keyword
    SerdecLexer* lex = make_lexer("0true");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 0);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_then_number) {
    SerdecLexer* lex = make_lexer("\"a\"123");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_STRING);
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(tok.number.value.u64, 123);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- 9. CRLF line tracking ---

TEST(lex_crlf_line_tracking) {
    // "{\r\n  @" — \r is whitespace (col++), \n is newline (line++)
    SerdecLexer* lex = make_lexer("{\r\n  @");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_EQ(err->line, 2);
    ASSERT_EQ(err->column, 3);
    serdec_lexer_destroy(lex);
}

// --- 10. UTF-8 / high bytes ---

// BUG DETECTOR: The control char check in lex_string uses:
//   if (*lexer->current < 0x20)
// On platforms where char is signed (macOS, most Linux), bytes >= 0x80
// (like UTF-8 multi-byte sequences) are negative, so -28 < 0x20 is true,
// causing valid UTF-8 strings to be rejected as control characters.
// Fix: cast to unsigned char: (unsigned char)*lexer->current < 0x20
TEST(lex_string_utf8_bytes) {
    SerdecLexer* lex = make_lexer("\"\xe4\xbd\xa0\xe5\xa5\xbd\""); // "你好"
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(tok.length, 6); // 3 bytes per CJK char
    serdec_lexer_destroy(lex);
}

TEST(lex_utf8_outside_string) {
    // High byte outside string is an unexpected character
    SerdecLexer* lex = make_lexer("\xe4");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

// --- A. Memory-safety boundaries ---

TEST(lex_string_very_long) {
    // 64KB string — stress scanning loop and length counter
    size_t content_len = 65536;
    char* input = (char*)malloc(content_len + 3);
    input[0] = '"';
    memset(input + 1, 'a', content_len);
    input[content_len + 1] = '"';
    input[content_len + 2] = '\0';
    SerdecBuffer* buf = serdec_buffer_from_string(input, content_len + 2);
    SerdecLexer* lex = serdec_lexer_create(buf);
    serdec_buffer_release(buf);
    free(input);
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(tok.length, content_len);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_very_long) {
    // 10K digit number — must overflow, not hang/crash
    size_t len = 10000;
    char* input = (char*)malloc(len + 1);
    memset(input, '9', len);
    input[len] = '\0';
    SerdecBuffer* buf = serdec_buffer_from_string(input, len);
    SerdecLexer* lex = serdec_lexer_create(buf);
    serdec_buffer_release(buf);
    free(input);
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_NUMBER_OVERFLOW);
    serdec_lexer_destroy(lex);
}

TEST(lex_deep_whitespace) {
    // 100KB of spaces before a token — no quadratic behavior
    size_t ws_len = 100 * 1024;
    char* input = (char*)malloc(ws_len + 5);
    memset(input, ' ', ws_len);
    memcpy(input + ws_len, "true", 4);
    input[ws_len + 4] = '\0';
    SerdecBuffer* buf = serdec_buffer_from_string(input, ws_len + 4);
    SerdecLexer* lex = serdec_lexer_create(buf);
    serdec_buffer_release(buf);
    free(input);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_TRUE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

// --- B. Peek/next full union equality ---

TEST(lex_peek_next_number_equality) {
    SerdecLexer* lex = make_lexer("-42");
    SerdecToken p = serdec_lexer_peek(lex);
    SerdecToken n = serdec_lexer_next(lex);
    ASSERT_EQ(p.type, n.type);
    ASSERT(p.start == n.start);
    ASSERT_EQ(p.length, n.length);
    ASSERT_EQ(p.number.is_integer, n.number.is_integer);
    ASSERT_EQ(p.number.is_negative, n.number.is_negative);
    ASSERT_EQ(p.number.value.i64, n.number.value.i64);
    serdec_lexer_destroy(lex);
}

TEST(lex_peek_next_string_equality) {
    SerdecLexer* lex = make_lexer("\"hello\\nworld\"");
    SerdecToken p = serdec_lexer_peek(lex);
    SerdecToken n = serdec_lexer_next(lex);
    ASSERT_EQ(p.type, n.type);
    ASSERT(p.start == n.start);
    ASSERT_EQ(p.length, n.length);
    ASSERT_EQ(p.string.has_escapes, n.string.has_escapes);
    serdec_lexer_destroy(lex);
}

// --- C. Error info correctness ---

TEST(lex_error_contents_stable) {
    // Error info unchanged after multiple next() calls (sticky error)
    SerdecLexer* lex = make_lexer("@");
    serdec_lexer_next(lex);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    SerdecError code = err->code;
    size_t line = err->line;
    size_t col = err->column;
    size_t offset = err->offset;
    serdec_lexer_next(lex);
    serdec_lexer_next(lex);
    ASSERT_EQ(err->code, code);
    ASSERT_EQ(err->line, line);
    ASSERT_EQ(err->column, col);
    ASSERT_EQ(err->offset, offset);
    serdec_lexer_destroy(lex);
}

TEST(lex_error_first_wins) {
    // "@#" — error always points to '@', not '#'
    SerdecLexer* lex = make_lexer("@#");
    serdec_lexer_next(lex);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_EQ(err->offset, 0);
    ASSERT_EQ(err->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_next(lex);
    ASSERT_EQ(err->offset, 0);
    serdec_lexer_destroy(lex);
}

// --- D. CR/LF semantics ---

TEST(lex_cr_alone_not_newline) {
    // \r alone is whitespace (column increment), NOT newline
    SerdecLexer* lex = make_lexer("{\r@");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_EQ(err->line, 1); // still line 1
    ASSERT_EQ(err->column, 3);
    serdec_lexer_destroy(lex);
}

TEST(lex_crlf_no_double_increment) {
    // \r\n counts as ONE line break (from \n), not two
    SerdecLexer* lex = make_lexer("{\r\n@");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_LBRACE);
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* err = serdec_lexer_get_error(lex);
    ASSERT_EQ(err->line, 2); // line 2, NOT line 3
    ASSERT_EQ(err->column, 1);
    serdec_lexer_destroy(lex);
}

// --- E. Spec-tight numbers ---

TEST(lex_number_neg_double_zero) {
    // -00 invalid (leading zero after minus)
    SerdecLexer* lex = make_lexer("-00");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_exp_leading_zero_neg) {
    // 1e-01 valid (exponent allows leading zeros)
    SerdecLexer* lex = make_lexer("1e-01");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 0.1) < 0.01);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_zero_trailing_dot) {
    // 0. invalid (no digits after dot)
    SerdecLexer* lex = make_lexer("0.");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_zero_dot_exp) {
    // 0.e1 invalid (no digits after dot before exponent)
    SerdecLexer* lex = make_lexer("0.e1");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_upper_e_plus_zero) {
    // 1E+0 valid
    SerdecLexer* lex = make_lexer("1E+0");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(!tok.number.is_integer);
    ASSERT(fabs(tok.number.value.f64 - 1.0) < 0.001);
    serdec_lexer_destroy(lex);
}

// --- F. More UTF-8 ---

TEST(lex_utf8_multibyte_outside_string) {
    // Full 3-byte UTF-8 outside string — error, not crash
    SerdecLexer* lex = make_lexer("\xe4\xbd\xa0");
    ASSERT_EQ(serdec_lexer_next(lex).type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

// NOTE: Will fail until signed-char bug is fixed in lex_string
TEST(lex_string_utf8_mixed_ascii) {
    // "hi你好ok" — mixed ASCII and UTF-8, length = byte count
    SerdecLexer* lex = make_lexer("\"hi\xe4\xbd\xa0\xe5\xa5\xbdok\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT_EQ(tok.length, 10); // 2 + 3 + 3 + 2
    serdec_lexer_destroy(lex);
}

// --- Deferred escape validation consistency ---

TEST(lex_string_unicode_escape_partial) {
    // "\u12" — lexer doesn't validate hex, just skips \\ + next char
    SerdecLexer* lex = make_lexer("\"\\u12\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_STRING);
    ASSERT(tok.string.has_escapes);
    serdec_lexer_destroy(lex);
}

// --- Error code taxonomy (lock down specific codes, not just ERROR type) ---

// BUG DETECTOR: Will FAIL — the bounds check in lex_string's backslash handler
// is dead code (current >= end is always false inside the while loop).
// The trailing backslash falls through to UNTERMINATED_STRING instead of INVALID_ESCAPE.
// Fix: change `if (lexer->current >= lexer->end)` to `if (lexer->current + 1 >= lexer->end)`
TEST(lex_string_error_code_backslash_at_end) {
    SerdecLexer* lex = make_lexer("\"abc\\");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_ESCAPE);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_error_code_leading_zero) {
    SerdecLexer* lex = make_lexer("01");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_INVALID_NUMBER);
    serdec_lexer_destroy(lex);
}

TEST(lex_string_error_code_control_char) {
    SerdecLexer* lex = make_lexer("\"hello\tworld\"");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_ERROR);
    ASSERT_EQ(serdec_lexer_get_error(lex)->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

// --- Peek: position and error stability ---

TEST(lex_peek_does_not_advance_position) {
    SerdecLexer* lex = make_lexer(" \n 42");
    (void)serdec_lexer_peek(lex);
    // peek internally calls next() and caches — next() should return the cached token
    // and NOT double-skip whitespace
    SerdecToken a = serdec_lexer_next(lex);
    SerdecToken b = serdec_lexer_next(lex);
    ASSERT_EQ(a.type, SERDEC_TOKEN_NUMBER);
    ASSERT_EQ(b.type, SERDEC_TOKEN_EOF);
    serdec_lexer_destroy(lex);
}

TEST(lex_peek_error_sticky) {
    SerdecLexer* lex = make_lexer("@");
    SerdecToken p = serdec_lexer_peek(lex);
    ASSERT_EQ(p.type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* e1 = serdec_lexer_get_error(lex);
    SerdecToken n = serdec_lexer_next(lex);
    ASSERT_EQ(n.type, SERDEC_TOKEN_ERROR);
    const SerdecErrorInfo* e2 = serdec_lexer_get_error(lex);
    ASSERT(e1 == e2);
    ASSERT_EQ(e1->code, SERDEC_ERR_UNEXPECTED_CHAR);
    serdec_lexer_destroy(lex);
}

// --- Structural invariant: all non-EOF tokens have start pointer ---

TEST(lex_non_eof_has_start_pointer) {
    SerdecLexer* lex = make_lexer("{\"a\":1}");
    for (;;) {
        SerdecToken t = serdec_lexer_next(lex);
        if (t.type == SERDEC_TOKEN_EOF) break;
        ASSERT_NOT_NULL(t.start);
        // All non-error tokens have length > 0, except empty strings
        if (t.type != SERDEC_TOKEN_ERROR)
            ASSERT(t.length > 0 || t.type == SERDEC_TOKEN_STRING);
    }
    serdec_lexer_destroy(lex);
}

// --- Golden table-driven test ---

// --- Phase 2 guide checklist gaps ---

TEST(lex_number_standalone_point_one) {
    SerdecLexer* lex = make_lexer("0.1");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer == false);
    ASSERT(fabs(tok.number.value.f64 - 0.1) < 1e-15);
    ASSERT_EQ(tok.length, 3);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_neg_float_upper_exp) {
    SerdecLexer* lex = make_lexer("-4.5E+20");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer == false);
    ASSERT(tok.number.is_negative == true);
    ASSERT(fabs(tok.number.value.f64 - (-4.5e+20)) < 1e+6);
    ASSERT_EQ(tok.length, 8);
    serdec_lexer_destroy(lex);
}

TEST(lex_number_locale_independent) {
    char* old_locale = setlocale(LC_NUMERIC, NULL);
    // Save a copy since the pointer may be invalidated
    char saved[64];
    snprintf(saved, sizeof(saved), "%s", old_locale);

    // Try setting a comma-decimal locale (may not be installed)
    char* result = setlocale(LC_NUMERIC, "de_DE.UTF-8");
    if (!result) result = setlocale(LC_NUMERIC, "fr_FR.UTF-8");

    // Parse 0.5 — must work regardless of locale
    SerdecLexer* lex = make_lexer("0.5");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer == false);
    ASSERT(fabs(tok.number.value.f64 - 0.5) < 1e-15);
    serdec_lexer_destroy(lex);

    // Restore
    setlocale(LC_NUMERIC, saved);
}

TEST(lex_number_underflow_denormal) {
    SerdecLexer* lex = make_lexer("1e-400");
    SerdecToken tok = serdec_lexer_next(lex);
    ASSERT_EQ(tok.type, SERDEC_TOKEN_NUMBER);
    ASSERT(tok.number.is_integer == false);
    ASSERT(tok.number.value.f64 == 0.0);
    serdec_lexer_destroy(lex);
}

TEST(lex_golden_sequence) {
    const char* input = "{\"key\":-0,\"arr\":[1,2.5,true,null]}";
    struct { SerdecTokenType type; size_t length; } expected[] = {
        { SERDEC_TOKEN_LBRACE,   1 },
        { SERDEC_TOKEN_STRING,   3 },  // key
        { SERDEC_TOKEN_COLON,    1 },
        { SERDEC_TOKEN_NUMBER,   2 },  // -0
        { SERDEC_TOKEN_COMMA,    1 },
        { SERDEC_TOKEN_STRING,   3 },  // arr
        { SERDEC_TOKEN_COLON,    1 },
        { SERDEC_TOKEN_LBRACKET, 1 },
        { SERDEC_TOKEN_NUMBER,   1 },  // 1
        { SERDEC_TOKEN_COMMA,    1 },
        { SERDEC_TOKEN_NUMBER,   3 },  // 2.5
        { SERDEC_TOKEN_COMMA,    1 },
        { SERDEC_TOKEN_TRUE,     4 },
        { SERDEC_TOKEN_COMMA,    1 },
        { SERDEC_TOKEN_NULL,     4 },
        { SERDEC_TOKEN_RBRACKET, 1 },
        { SERDEC_TOKEN_RBRACE,   1 },
        { SERDEC_TOKEN_EOF,      0 },
    };
    SerdecLexer* lex = make_lexer(input);
    for (int i = 0; i < 18; i++) {
        SerdecToken tok = serdec_lexer_next(lex);
        ASSERT_EQ(tok.type, expected[i].type);
        if (tok.type != SERDEC_TOKEN_EOF)
            ASSERT_EQ(tok.length, expected[i].length);
    }
    serdec_lexer_destroy(lex);
}

int test_lexer(void) {
    printf("\n  Lexer tests:\n");

    // Structural
    RUN(lex_structural);
    RUN(lex_structural_start_length);

    // Whitespace
    RUN(lex_whitespace);
    RUN(lex_only_whitespace);
    RUN(lex_empty_input);

    // Keywords
    RUN(lex_keywords);
    RUN(lex_keyword_prefix_reject);
    RUN(lex_keyword_case_sensitive);
    RUN(lex_keyword_at_end);
    RUN(lex_keyword_truncated);
    RUN(lex_keyword_followed_by_structural);
    RUN(lex_keyword_with_digit_suffix);

    // Strings
    RUN(lex_string_simple);
    RUN(lex_string_empty);
    RUN(lex_string_with_escapes);
    RUN(lex_string_escaped_quote);
    RUN(lex_string_escaped_backslash);
    RUN(lex_string_unterminated);
    RUN(lex_string_control_char);
    RUN(lex_string_newline_reject);
    RUN(lex_string_null_byte);
    RUN(lex_string_all_escape_types);
    RUN(lex_string_unicode_escape);
    RUN(lex_string_backslash_at_end);
    RUN(lex_string_only_escapes);
    RUN(lex_string_escaped_backslash_before_quote);
    RUN(lex_string_multiple_sequential);
    RUN(lex_string_just_quote);
    RUN(lex_string_carriage_return);

    // Numbers: integers
    RUN(lex_number_zero);
    RUN(lex_number_simple_int);
    RUN(lex_number_negative_int);
    RUN(lex_number_negative_zero);
    RUN(lex_number_large_int);
    RUN(lex_number_uint64_max);
    RUN(lex_number_uint64_overflow);
    RUN(lex_number_int64_min);
    RUN(lex_number_int64_overflow);
    RUN(lex_number_single_digit_each);

    // Numbers: floats
    RUN(lex_number_simple_float);
    RUN(lex_number_negative_float);
    RUN(lex_number_zero_point);
    RUN(lex_number_zero_exponent);
    RUN(lex_number_negative_zero_float);
    RUN(lex_number_negative_with_exponent);
    RUN(lex_number_many_fraction_digits);
    RUN(lex_number_exponent_e0);
    RUN(lex_number_frac_exponent_negative);

    // Numbers: scientific notation
    RUN(lex_number_exponent);
    RUN(lex_number_exponent_upper);
    RUN(lex_number_exponent_positive);
    RUN(lex_number_exponent_negative);
    RUN(lex_number_fraction_and_exponent);
    RUN(lex_number_large_exponent);
    RUN(lex_number_small_exponent);

    // Numbers: invalid
    RUN(lex_number_leading_zero);
    RUN(lex_number_leading_zero_neg);
    RUN(lex_number_trailing_dot);
    RUN(lex_number_leading_dot);
    RUN(lex_number_missing_exp_digits);
    RUN(lex_number_explicit_plus);
    RUN(lex_number_double_minus);
    RUN(lex_number_minus_alone);
    RUN(lex_number_multiple_dots);
    RUN(lex_number_exp_no_sign_digits);
    RUN(lex_number_hex_reject);
    RUN(lex_number_octal_reject);
    RUN(lex_number_dot_exp);
    RUN(lex_number_double_exp);
    RUN(lex_number_minus_dot);
    RUN(lex_number_minus_e);
    RUN(lex_number_huge_exponent);

    // Numbers: in context
    RUN(lex_number_followed_by_comma);
    RUN(lex_number_followed_by_brace);
    RUN(lex_number_followed_by_bracket);
    RUN(lex_number_followed_by_colon);
    RUN(lex_number_followed_by_whitespace);

    // Peek
    RUN(lex_peek);
    RUN(lex_peek_at_eof);
    RUN(lex_peek_preserves_value);
    RUN(lex_peek_next_alternating);
    RUN(lex_eof_repeatable);

    // Full JSON
    RUN(lex_full_object);
    RUN(lex_full_array);

    // Edge cases
    RUN(lex_unexpected_char);
    RUN(lex_unexpected_chars_variety);
    RUN(lex_null_safety);
    RUN(lex_create_null_buffer);

    // Full JSON: complex
    RUN(lex_nested_json);
    RUN(lex_all_value_types);

    // Line/column tracking
    RUN(lex_error_position_line_column);
    RUN(lex_error_position_offset);

    // 1. Token span + length invariants
    RUN(lex_structural_all_length_one);
    RUN(lex_keyword_span);
    RUN(lex_number_token_length);

    // 2. Peek/next identity
    RUN(lex_peek_pointer_identity);
    RUN(lex_peek_eof_identity);

    // 3. Error state contract
    RUN(lex_error_sticky_unexpected);
    RUN(lex_error_info_populated);
    RUN(lex_error_info_stable);

    // 5. String escape edge cases
    RUN(lex_string_escape_validation_deferred);
    RUN(lex_string_escaped_slash);

    // 6. Number grammar edge cases
    RUN(lex_number_exponent_leading_zero);
    RUN(lex_number_zero_exp_variants);
    RUN(lex_number_trailing_zeros_valid);
    RUN(lex_number_double_sign_exp);
    RUN(lex_number_float_infinity);
    RUN(lex_number_negative_zero_contract);

    // 7. Grammar agnostic
    RUN(lex_grammar_agnostic);

    // 8. Adjacency
    RUN(lex_adjacent_keyword_keyword);
    RUN(lex_zero_then_keyword);
    RUN(lex_string_then_number);

    // 9. CRLF
    RUN(lex_crlf_line_tracking);

    // 10. UTF-8
    RUN(lex_string_utf8_bytes);
    RUN(lex_utf8_outside_string);

    // A. Memory-safety boundaries
    RUN(lex_string_very_long);
    RUN(lex_number_very_long);
    RUN(lex_deep_whitespace);

    // B. Peek/next full union equality
    RUN(lex_peek_next_number_equality);
    RUN(lex_peek_next_string_equality);

    // C. Error info correctness
    RUN(lex_error_contents_stable);
    RUN(lex_error_first_wins);

    // D. CR/LF semantics
    RUN(lex_cr_alone_not_newline);
    RUN(lex_crlf_no_double_increment);

    // E. Spec-tight numbers
    RUN(lex_number_neg_double_zero);
    RUN(lex_number_exp_leading_zero_neg);
    RUN(lex_number_zero_trailing_dot);
    RUN(lex_number_zero_dot_exp);
    RUN(lex_number_upper_e_plus_zero);

    // F. More UTF-8
    RUN(lex_utf8_multibyte_outside_string);
    RUN(lex_string_utf8_mixed_ascii);

    // Deferred escape validation
    RUN(lex_string_unicode_escape_partial);

    // Error code taxonomy
    RUN(lex_string_error_code_backslash_at_end);
    RUN(lex_number_error_code_leading_zero);
    RUN(lex_string_error_code_control_char);

    // Peek position & error stability
    RUN(lex_peek_does_not_advance_position);
    RUN(lex_peek_error_sticky);

    // Structural invariant
    RUN(lex_non_eof_has_start_pointer);

    // Phase 2 guide checklist gaps
    RUN(lex_number_standalone_point_one);
    RUN(lex_number_neg_float_upper_exp);
    RUN(lex_number_locale_independent);
    RUN(lex_number_underflow_denormal);

    // Golden test
    RUN(lex_golden_sequence);

    TEST_SUMMARY();
}
