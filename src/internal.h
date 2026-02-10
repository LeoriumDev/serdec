#pragma once

#include <serdec/types.h>

struct SerdecBuffer {
    uint32_t magic;           // 0x5EDEC001 for validation
    uint32_t ref_count;
    char *data;               // 64-byte aligned
    size_t size;
    size_t capacity;          // size + padding
};
