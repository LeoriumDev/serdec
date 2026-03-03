#include "internal.h"
#include "serdec/error.h"

SerdecError serdec_string_unescape(SerdecArena* arena, const char* src, size_t len,
                                    char** out, size_t* out_len) {
    if (!arena || !src || !out || !out_len) return SERDEC_ERR_INVALID_ESCAPE;
    const char* ptr = src;
    while (*ptr != '\0') {
        
    }
}
