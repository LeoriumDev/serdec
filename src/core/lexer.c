#include "internal.h"
#include <string.h>

static SerdecToken make_error(SerdecLexer* lexer, SerdecError code) {
    // TODO
    return (SerdecToken){ .type = SERDEC_TOKEN_ERROR };
}

static void skip_whitespace(SerdecLexer* lexer) {
    // TODO
}

static SerdecToken lex_keyword(SerdecLexer* lexer, const char* keyword, SerdecTokenType type) {
    // TODO
    return make_error(lexer, SERDEC_ERR_INVALID_VALUE);
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
    // TODO
    return NULL;
}

void serdec_lexer_destroy(SerdecLexer* lexer) {
    // TODO
}

const SerdecErrorInfo* serdec_lexer_get_error(const SerdecLexer* lexer) {
    // TODO
    return NULL;
}

SerdecToken serdec_lexer_next(SerdecLexer* lexer) {
    // TODO
    return (SerdecToken){ .type = SERDEC_TOKEN_EOF };
}

SerdecToken serdec_lexer_peek(SerdecLexer* lexer) {
    // TODO
    return (SerdecToken){ .type = SERDEC_TOKEN_EOF };
}
