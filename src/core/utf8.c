#include "internal.h"

int serdec_utf8_decode(const char* data, size_t len, uint32_t* codepoint) {
    (void)data; (void)len; (void)codepoint;
    return 0; // TODO
}

int serdec_utf8_encode(uint32_t codepoint, char* out) {
    (void)codepoint; (void)out;
    return 0; // TODO
}

bool serdec_utf8_validate(const char* data, size_t len) {
    (void)data; (void)len;
    return false; // TODO
}
