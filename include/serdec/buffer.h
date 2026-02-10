#pragma once

#include <serdec/types.h>
#include <stdio.h>

// Create buffer
SerdecBuffer* serdec_buffer_from_string(const char* json, size_t len);
SerdecBuffer* serdec_buffer_from_file(const char* path, size_t max_size);
SerdecBuffer* serdec_buffer_from_stream(FILE* fp, size_t max_size);

// Reference counting
SerdecBuffer* serdec_buffer_retain(SerdecBuffer* buf);
void serdec_buffer_release(SerdecBuffer* buf);             // Decrements refcount, frees at 0

// Access (read-only)
const char* serdec_buffer_data(const SerdecBuffer* buf);
size_t serdec_buffer_size(const SerdecBuffer* buf);        // Logical size
size_t serdec_buffer_capacity(const SerdecBuffer* buf);    // size + padding
