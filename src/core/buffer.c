#include "internal.h"
#include <stdio.h>

SerdecBuffer* serdec_buffer_from_string(const char* json, size_t len) {
    return NULL;
}

SerdecBuffer* serdec_buffer_from_file(const char* path, size_t max_size) {
    return NULL;
}

SerdecBuffer* serdec_buffer_from_stream(FILE* fp, size_t max_size) {
    return NULL;
}

SerdecBuffer* serdec_buffer_retain(SerdecBuffer* buf) {
    return NULL;
}

void serdec_buffer_release(SerdecBuffer* buf) {
    return;
}

const char* serdec_buffer_data(const SerdecBuffer* buf) {
    return NULL;
}

size_t serdec_buffer_size(const SerdecBuffer* buf) {
    return 0;
}

size_t serdec_buffer_capacity(const SerdecBuffer* buf) {
    return 0;
}
