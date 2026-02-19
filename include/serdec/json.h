#pragma once

#include <serdec/types.h>
#include <serdec/error.h>

/**
 * @brief Create a JSON event iterator parser.
 *
 * @param input Pointer to input buffer.
 * @param len   Length of input in bytes.
 * @return New parser, or NULL on allocation failure.
 *
 * @note The input buffer must outlive all SerdecString values produced
 *       from this parser.
 */
SerdecParser* serdec_json_parser_create(const char* input, size_t len);

/**
 * @brief Destroy a parser and free its resources.
 *
 * @param parser Parser to destroy.
 */
void serdec_json_parser_destroy(SerdecParser* parser);

/**
 * @brief Advance to the next event.
 *
 * @param parser Parser instance.
 * @param ev     Output event. Valid until the next call.
 * @return SERDEC_OK on success, or an error code.
 *         On error, ev->kind is set to SERDEC_EVENT_ERROR.
 */
SerdecError serdec_json_event_next(SerdecParser* parser, SerdecEvent* ev);

/**
 * @brief Retrieve the last error detail from the parser.
 *
 * @param parser Parser instance.
 * @return Pointer to error info, valid for the lifetime of the parser.
 */
const SerdecErrorInfo* serdec_json_parser_error(const SerdecParser* parser);
