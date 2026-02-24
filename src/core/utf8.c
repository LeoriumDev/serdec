#include "internal.h"
#include <stddef.h>
#include <stdint.h>

static bool is_continuation(uint8_t byte) {
    return byte >= 0x80 && byte <= 0xBF;
}

int serdec_utf8_decode(const char* data, size_t len, uint32_t* codepoint) {
    if (!data || !len || !codepoint) return 0;

    uint8_t b0 = (uint8_t) data[0];
    uint8_t b1, b2, b3;
    uint32_t cp;
    size_t width = 0;

    if (b0 <= 0x7F) {
        width = 1; // ASCII
    } else if (b0 <= 0xBF) {
        return 0;  // bare continuation
    } else if (b0 <= 0xC1) {
        return 0;  // overlong
    } else if (b0 <= 0xDF) {
        width = 2;
    } else if (b0 <= 0xEF) {
        width = 3;
    } else if (b0 <= 0xF4) {
        width = 4;
    } else {
        return 0;  // out of range
    }

    if (len < width) return 0;

    if (width == 1) {
        cp = b0 & 0x7F;
    } else if (width == 2) {
        b1 = (uint8_t) data[1];
        if (!is_continuation(b1)) return 0;
        cp = (b0 & 0x1F) << 6 | (b1 & 0x3F);
        if (cp < 0x80) return 0;
    } else if (width == 3) {
        b1 = (uint8_t) data[1];
        b2 = (uint8_t) data[2];
        if (!is_continuation(b1)) return 0;
        if (!is_continuation(b2)) return 0;
        cp = (b0 & 0x0F) << 12 | (b1 & 0x3F) << 6 | (b2 & 0x3F);
        if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) return 0;
    } else {
        b1 = (uint8_t) data[1];
        b2 = (uint8_t) data[2];
        b3 = (uint8_t) data[3];

        if (!is_continuation(b1)) return 0;
        if (!is_continuation(b2)) return 0;
        if (!is_continuation(b3)) return 0;
        cp = (b0 & 0x07) << 18 | (b1 & 0x3F) << 12 | (b2 & 0x3F) << 6 | (b3 & 0x3F);
        if (cp < 0x10000 || cp > 0x10FFFF) return 0;
    }
    *codepoint = cp;
    return width;
}

int serdec_utf8_encode(uint32_t codepoint, char* out) {
    if (!out) return 0;
    if (codepoint > 0x10FFFF) return 0;
    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) return 0;

    if (codepoint <= 0x7F) {
        out[0] = codepoint;
        return 1;
    } else if (codepoint <= 0x7FF) {
        out[0] = 0xC0 | (codepoint >> 6);
        out[1] = 0x80 | (codepoint & 0x3F);
        return 2;
    } else if (codepoint <= 0xFFFF) {
        out[0] = 0xE0 | (codepoint >> 12);
        out[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        out[2] = 0x80 | (codepoint & 0x3F);
        return 3;
    } else {
        out[0] = 0xF0 | (codepoint >> 18);
        out[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        out[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        out[3] = 0x80 | (codepoint & 0x3F);
        return 4;
    }
}

bool serdec_utf8_validate(const char* data, size_t len) {
    if (!data) return false;

    size_t i = 0;
    while (i < len) {
        uint32_t cp;
        int n = serdec_utf8_decode(data + i, len - i, &cp);
        if (n == 0) return false;
        i += n;
    }

    return true;
}
