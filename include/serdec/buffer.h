#pragma once

#include <serdec/types.h>
#include <stdio.h>

/**
 * @brief Create a buffer from a string.
 *
 * @param str Source string.
 * @param len Length of source string in bytes.
 * @return New buffer with refcount 1, or NULL on failure.
 */
SerdecBuffer* serdec_buffer_from_string(const char* str, size_t len);

/**
 * @brief Create a buffer by reading a file.
 *
 * @param path     Path to the file.
 * @param max_size Maximum bytes to read. 0 for no limit.
 * @return New buffer with refcount 1, or NULL on failure.
 */
SerdecBuffer* serdec_buffer_from_file(const char* path, size_t max_size);

/**
 * @brief Create a buffer by reading from a stream.
 *
 * @param fp       Input stream.
 * @param max_size Maximum bytes to read. 0 for no limit.
 * @return New buffer with refcount 1, or NULL on failure.
 */
SerdecBuffer* serdec_buffer_from_stream(FILE* fp, size_t max_size);

/**
 * @brief Increment the buffer's reference count.
 *
 * @param buf Buffer to retain.
 * @return The same buffer.
 */
SerdecBuffer* serdec_buffer_retain(SerdecBuffer* buf);

/**
 * @brief Decrement the buffer's reference count. Frees the buffer when it reaches 0.
 *
 * @param buf Buffer to release.
 */
void serdec_buffer_release(SerdecBuffer* buf);

/**
 * @brief Return a read-only pointer to the buffer's data.
 *
 * @param buf Buffer to query.
 * @return Pointer to buffer data.
 */
const char* serdec_buffer_data(const SerdecBuffer* buf);

/**
 * @brief Return the logical size of the buffer in bytes.
 *
 * @param buf Buffer to query.
 * @return Logical size in bytes.
 */
size_t serdec_buffer_size(const SerdecBuffer* buf);

/**
 * @brief Return the capacity of the buffer in bytes (size + padding).
 *
 * @param buf Buffer to query.
 * @return Capacity in bytes.
 */
size_t serdec_buffer_capacity(const SerdecBuffer* buf);
