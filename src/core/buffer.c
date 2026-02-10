#include "serdec/types.h"
#include "internal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PADDING 64

SerdecBuffer* serdec_buffer_from_string(const char* str, size_t len) {
    if (!str) return NULL;

    SerdecBuffer* buf = malloc(sizeof(*buf));
    if (!buf) return NULL;

    *buf = (SerdecBuffer) {
        .magic = SERDEC_MAGIC_BUFFER,
        .ref_count = 1,
        .data = NULL,
        .size = len,
        .capacity = (SERDEC_DEFAULT_BUFFER_CAPACITY > len) ? SERDEC_DEFAULT_BUFFER_CAPACITY : len
    };

    buf->data = serdec_aligned_alloc(64, buf->capacity + PADDING);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    memset(buf->data, 0, buf->capacity + PADDING);
    memcpy(buf->data, str, len);
    return buf;
}

SerdecBuffer* serdec_buffer_from_file(const char* path, size_t max_size) {
    return NULL;
}

SerdecBuffer* serdec_buffer_from_stream(FILE* fp, size_t max_size) {
    return NULL;
}

SerdecBuffer* serdec_buffer_retain(SerdecBuffer* buf) {
    if (!buf || buf->magic != SERDEC_MAGIC_BUFFER) return NULL;
    buf->ref_count++;
    return buf;
}

void serdec_buffer_release(SerdecBuffer* buf) {
    if (!buf || buf->magic != SERDEC_MAGIC_BUFFER) return;

    buf->ref_count--;

    if (buf->ref_count == 0) {
        buf->magic = SERDEC_MAGIC_FREED;
        serdec_aligned_free(buf->data);
        free(buf);
    }
}

const char* serdec_buffer_data(const SerdecBuffer* buf) {
    if (!buf || buf->magic != SERDEC_MAGIC_BUFFER) return NULL;
    return buf->data;
}

size_t serdec_buffer_size(const SerdecBuffer* buf) {
    if (!buf || buf->magic != SERDEC_MAGIC_BUFFER) return SIZE_MAX;
    return buf->size;
}

size_t serdec_buffer_capacity(const SerdecBuffer* buf) {
    if (!buf || buf->magic != SERDEC_MAGIC_BUFFER) return SIZE_MAX;
    return buf->capacity;
}
