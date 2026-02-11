#pragma once

#include <serdec/serdec.h>

#define SERDEC_MAGIC_BUFFER 0x5EDEC00B
#define SERDEC_MAGIC_ARENA  0x5EDEC00A
#define SERDEC_MAGIC_FREED  0xDEADBEEF

#define SERDEC_DEFAULT_BUFFER_CAPACITY 100

#ifdef _WIN32
    #include <malloc.h>                                                       
    #define serdec_aligned_alloc(align, size) _aligned_malloc(size, align)    
    #define serdec_aligned_free(ptr)          _aligned_free(ptr)              
#else                                                                         
    #include <stdlib.h>                                                       
    static inline void* serdec_aligned_alloc(size_t align, size_t size) {     
        void* ptr = NULL;                                                     
        posix_memalign(&ptr, align, size);                                    
        return ptr;                                                           
    }                                                                         
    #define serdec_aligned_free(ptr) free(ptr)                                
#endif

struct SerdecBuffer {
    uint32_t magic;           // 0x5EDEC00B for validation
    uint32_t ref_count;
    char* data;               // 64-byte aligned
    size_t size;
    size_t capacity;          // size + padding
};

typedef struct ArenaBlock {
    struct ArenaBlock* next;
    size_t size;              // Block capacity
    size_t used;              // Bytes used
    char data[];              // Flexible array member (C99)
} ArenaBlock;

struct SerdecArena {
    uint32_t magic;           // 0x5EDEC00A for validation
    ArenaBlock* current;      // Current allocation block
    ArenaBlock* first;        // Keep for reset
    size_t total_allocated;   // For limit checking
    SerdecArenaConfig config;
};

typedef enum SerdecTokenType {
    SERDEC_TOKEN_LBRACE,      // {
    SERDEC_TOKEN_RBRACE,      // }
    SERDEC_TOKEN_LBRACKET,    // [
    SERDEC_TOKEN_RBRACKET,    // ]
    SERDEC_TOKEN_COLON,       // :
    SERDEC_TOKEN_COMMA,       // ,
    SERDEC_TOKEN_STRING,      // "..."
    SERDEC_TOKEN_NUMBER,      // 123, -45.6, 1.2e-3
    SERDEC_TOKEN_TRUE,        // true
    SERDEC_TOKEN_FALSE,       // false
    SERDEC_TOKEN_NULL,        // null
    SERDEC_TOKEN_EOF,         // End of input
    SERDEC_TOKEN_ERROR        // Lexer error
} SerdecTokenType;

typedef struct SerdecToken {
    SerdecTokenType type;
    const char *start;        // Points into original buffer
    size_t length;            // Length of token text

    union {
        struct {              // For TOKEN_NUMBER
            bool is_integer;
            bool is_negative;
            union {
                int64_t i64;
                uint64_t u64;
                double f64;
            } value;
        } number;

        struct {              // For TOKEN_STRING
            bool has_escapes; // Needs unescaping?
        } string;
    };
} SerdecToken;

typedef struct SerdecLexer {
    const char *input;        // Start of buffer
    const char *current;      // Current position
    const char *end;          // End of logical input

    size_t line;              // Current line (1-indexed)
    size_t column;            // Current column (1-indexed)

    SerdecToken peeked;       // For peek() implementation
    bool has_peeked;

    SerdecErrorInfo error;
} SerdecLexer;

// Lexer API
SerdecLexer* serdec_lexer_create(SerdecBuffer* buf);
void serdec_lexer_destroy(SerdecLexer* lexer);
SerdecToken serdec_lexer_next(SerdecLexer* lexer);
SerdecToken serdec_lexer_peek(SerdecLexer* lexer);
const SerdecErrorInfo* serdec_lexer_get_error(const SerdecLexer* lexer);
