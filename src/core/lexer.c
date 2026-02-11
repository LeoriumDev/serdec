#include "internal.h"
#include <stddef.h>
#include <string.h>

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
    // TODO
    return make_error(lexer, SERDEC_ERR_UNTERMINATED_STRING);
}

static SerdecToken lex_number(SerdecLexer* lexer) {
    // TODO
    return make_error(lexer, SERDEC_ERR_INVALID_NUMBER);
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
