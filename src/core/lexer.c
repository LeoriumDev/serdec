#include "internal.h"
#include "serdec/error.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

static SerdecToken make_error(SerdecLexer* lexer, SerdecError code) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };
    
    lexer->error = (SerdecErrorInfo) {
        .code = code,
        .offset = lexer->current - lexer->start,
        .line = lexer->line,
        .column = lexer->column,
    };

    return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };
}

static SerdecToken make_structural(SerdecLexer* lexer, SerdecTokenType type) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };

    const char* start = lexer->current++;
    lexer->column++;
    return (SerdecToken) { .type = type, .start = start, .length = 1 };
}

static void skip_whitespace(SerdecLexer* lexer) {
    if (!lexer) return;

    while (lexer->current < lexer->end) {
        switch (*lexer->current) {
        case ' ':
        case '\t':
        case '\r': {
            lexer->column++;
            break;
        }
        case '\n': {
            lexer->column = 1;
            lexer->line++;
            break;
        }
        default: return;
        }
        lexer->current++;
    }
}

static bool is_alnum(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static SerdecToken lex_keyword(SerdecLexer* lexer, const char* keyword, SerdecTokenType type) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };
    const char* start_pos = lexer->current;
    size_t keyword_len = strlen(keyword);

    if (lexer->current + keyword_len > lexer->end)
        return make_error(lexer, SERDEC_ERR_INVALID_VALUE);

    if (memcmp(lexer->current, keyword, keyword_len) != 0)
        return make_error(lexer, SERDEC_ERR_INVALID_VALUE);

    // Safe to read current[keyword_len] even at end of input:
    // buffer has 64 bytes of zero padding, so is_alnum('\0') returns false.
    if (is_alnum(lexer->current[keyword_len]))
        return make_error(lexer, SERDEC_ERR_INVALID_VALUE);

    lexer->current += keyword_len;
    lexer->column += keyword_len;

    return (SerdecToken) {
        .type = type,
        .start = start_pos,
        .length = keyword_len,
    };
}

static SerdecToken lex_string(SerdecLexer* lexer) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };

    const char* start_pos = lexer->current + 1;
    bool has_escapes = false;
    while (++lexer->current < lexer->end) {
        lexer->column++;

        // Control characters (bytes 0x00 to 0x1F)
        if ((unsigned char) *lexer->current < 0x20)
            return make_error(lexer, SERDEC_ERR_UNEXPECTED_CHAR);
        
        // Escape sequence
        if (*lexer->current == '\\') {
            // Not enough room for trailing quote
            if (lexer->current + 1 >= lexer->end)
                return make_error(lexer, SERDEC_ERR_INVALID_ESCAPE);

            // Skip next character
            lexer->current++;
            lexer->column++;
            has_escapes = true;
            continue;
        }
        
        if (*lexer->current == '"') {
            lexer->column++; // Advance to the next character
            return (SerdecToken) {
                .type = SERDEC_TOKEN_STRING,
                .start = start_pos,
                // Calculate legnth before advancing 
                .length = lexer->current++ - start_pos,
                .string = { has_escapes }
            };
        }
    }

    return make_error(lexer, SERDEC_ERR_UNTERMINATED_STRING);
}

static SerdecToken lex_number(SerdecLexer* lexer) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };

    const char* start_pos = lexer->current;
    const char* digit_ptr = start_pos;
    bool is_negative = false;
    bool is_float = false;
    bool negative_exp = false;

    if (*lexer->current == '-') {
        lexer->current++;
        is_negative = true;
        digit_ptr = lexer->current;
        if (!is_digit(*lexer->current)) return make_error(lexer, SERDEC_ERR_INVALID_NUMBER);
    } 
    
    if (*lexer->current == '0') {
        lexer->current++;
        if (is_digit(*lexer->current)) return make_error(lexer, SERDEC_ERR_INVALID_NUMBER);
    }

    while (is_digit(*lexer->current)) lexer->current++;

    if (*lexer->current == '.') {
        lexer->current++;
        is_float = true;
        if (!is_digit(*lexer->current)) return make_error(lexer, SERDEC_ERR_INVALID_NUMBER);
        while (is_digit(*lexer->current)) lexer->current++;
    }
    
    if (*lexer->current == 'e' || *lexer->current == 'E') {
        lexer->current++;
        is_float = true;

        // Optional '+' and '-'
        if (*lexer->current == '+') {
            lexer->current++;
            negative_exp = false;
        } else if (*lexer->current == '-') {
            lexer->current++;
            negative_exp = true;
        }

        if (!is_digit(*lexer->current)) { return make_error(lexer, SERDEC_ERR_INVALID_NUMBER); }
        while (is_digit(*lexer->current)) lexer->current++;
    }

    size_t length = lexer->current - start_pos;
    lexer->column += length;

    // integer conversion

    uint64_t u64 = 0;
    int64_t i64 = 0;
    const char* digit_ptr1 = digit_ptr;

    if (!is_float) {
        while (is_digit(*digit_ptr1)) {
            if (u64 > UINT64_MAX / 10 || (u64 == UINT64_MAX / 10 && ((unsigned long long) *digit_ptr1 - '0') > UINT64_MAX % 10))
                return make_error(lexer, SERDEC_ERR_NUMBER_OVERFLOW);
            u64 = u64 * 10 + (*digit_ptr1 - '0');
            digit_ptr1++;
        }

        if (is_negative && u64 > (uint64_t)INT64_MAX + 1)
            return make_error(lexer, SERDEC_ERR_NUMBER_OVERFLOW);

        if (is_negative) i64 = (int64_t) (~u64 + 1);
    }

    // float conversion

    uint64_t whole = 0;
    double frac = 0.0;
    double divisor = 10.0;
    int64_t power = 0;
    double f64 = 0.0;
    const char* digit_ptr2 = digit_ptr;

    if (is_float) {
        while (*digit_ptr2 != '.' &&
               *digit_ptr2 != 'e' &&
               *digit_ptr2 != 'E' &&
               is_digit(*digit_ptr2)) {
            if (whole > UINT64_MAX / 10 || (whole == UINT64_MAX / 10 && ((unsigned long long) *digit_ptr2 - '0') > UINT64_MAX % 10))
                return make_error(lexer, SERDEC_ERR_NUMBER_OVERFLOW);
            whole = whole * 10 + (*digit_ptr2 - '0');
            digit_ptr2++;
        }
        if (*digit_ptr2 == '.') {
            digit_ptr2++;
            while (is_digit(*digit_ptr2)) {
                frac += (*digit_ptr2 - '0') / divisor;
                divisor *= 10.0;
                digit_ptr2++;
            }
        }
        
        if (*digit_ptr2 == 'e' || *digit_ptr2 == 'E') {
            digit_ptr2++;
            if (*digit_ptr2 == '-' || *digit_ptr2 == '+')
                digit_ptr2++;
            while (is_digit(*digit_ptr2)) {
                if (power > 308) { power = 309; break; }
                power = power * 10 + (*digit_ptr2 - '0');
                digit_ptr2++;
            }
            if (negative_exp) power = -power;
        }

        f64 = (whole + frac);
        if (power != 0) f64 *= pow(10.0, power);
        if (is_negative) f64 = -f64;
    }

    SerdecToken tok = { .type = SERDEC_TOKEN_NUMBER, .start = start_pos, .length = length };
    tok.number.is_integer = !is_float;
    tok.number.is_negative = is_negative;
    if (is_float) tok.number.value.f64 = f64;
    else if (is_negative) tok.number.value.i64 = i64;
    else tok.number.value.u64 = u64;

    return tok;
}

SerdecLexer* serdec_lexer_create(SerdecBuffer* buf) {
    if (!buf || buf->magic != SERDEC_MAGIC_BUFFER) return NULL;

    SerdecLexer* lexer = (SerdecLexer*) malloc(sizeof(*lexer));
    if (!lexer) return NULL;

    serdec_buffer_retain(buf);
    *lexer = (SerdecLexer) {
        .start = buf->data,
        .current = buf->data,
        .end = buf->data + buf->size,
        .line = 1,
        .column = 1,
        .has_peeked = false,
        .buffer = buf
    };

    return lexer;
}

void serdec_lexer_destroy(SerdecLexer* lexer) {
    if (!lexer) return;
    serdec_buffer_release(lexer->buffer);
    free(lexer);
}

const SerdecErrorInfo* serdec_lexer_get_error(const SerdecLexer* lexer) {
    if (!lexer) return NULL;
    return &lexer->error;
}

SerdecToken serdec_lexer_next(SerdecLexer* lexer) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };

    if (lexer->has_peeked) {
        lexer->has_peeked = false;
        return lexer->peeked;
    }

    skip_whitespace(lexer);

    if (lexer->current >= lexer->end)
        return (SerdecToken) { .type = SERDEC_TOKEN_EOF };

    char chr = *lexer->current;
    switch (chr) {
    // Structural Character Delegation
    case '{': return make_structural(lexer, SERDEC_TOKEN_LBRACE);
    case '}': return make_structural(lexer, SERDEC_TOKEN_RBRACE);
    case '[': return make_structural(lexer, SERDEC_TOKEN_LBRACKET);
    case ']': return make_structural(lexer, SERDEC_TOKEN_RBRACKET);
    case ':': return make_structural(lexer, SERDEC_TOKEN_COLON);
    case ',': return make_structural(lexer, SERDEC_TOKEN_COMMA);

    // String Delegation
    case '"': return lex_string(lexer);
    
    // Keyword Delegation (true, false, null)
    case 't': return lex_keyword(lexer, "true", SERDEC_TOKEN_TRUE);
    case 'f': return lex_keyword(lexer, "false", SERDEC_TOKEN_FALSE);
    case 'n': return lex_keyword(lexer, "null", SERDEC_TOKEN_NULL);

    // Number Delegation (minus sign, 0-9)
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
        return lex_number(lexer);
    }

    default: return make_error(lexer, SERDEC_ERR_UNEXPECTED_CHAR);
    }
}

SerdecToken serdec_lexer_peek(SerdecLexer* lexer) {
    if (!lexer) return (SerdecToken) { .type = SERDEC_TOKEN_ERROR };

    if (lexer->has_peeked)
        return lexer->peeked;

    lexer->peeked = serdec_lexer_next(lexer);
    lexer->has_peeked = true;

    return lexer->peeked;
}
