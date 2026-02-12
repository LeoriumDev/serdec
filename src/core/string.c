#include "internal.h"
#include "serdec/error.h"

SerdecError serdec_string_unescape(SerdecArena* arena, const char* src, size_t len,
                                    char** out, size_t* out_len) {
    (void)arena; (void)src; (void)len; (void)out; (void)out_len;
    return SERDEC_OK; // TODO
}
