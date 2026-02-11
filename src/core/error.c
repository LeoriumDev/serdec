#include "internal.h"
#include <stddef.h>
#include <stdio.h>

const char* serdec_error_string(SerdecError code) {
    switch (code) {
    case SERDEC_OK:                      return "OK";

    case SERDEC_ERR_UNEXPECTED_CHAR:     return "Unexpected Character";
    case SERDEC_ERR_UNEXPECTED_EOF:      return "Unexpected EOF";
    case SERDEC_ERR_INVALID_VALUE:       return "Invalid Value";
    case SERDEC_ERR_TRAILING_CHARS:      return "Trailing Characters";

    case SERDEC_ERR_INVALID_ESCAPE:      return "Invalid Escape";
    case SERDEC_ERR_INVALID_UTF8:        return "Invalid UTF-8";
    case SERDEC_ERR_UNTERMINATED_STRING: return "Unterminated String";

    case SERDEC_ERR_INVALID_NUMBER:      return "Invalid Number";
    case SERDEC_ERR_NUMBER_OVERFLOW:     return "Number Overflow";

    case SERDEC_ERR_DEPTH_LIMIT:         return "Depth Limit";
    case SERDEC_ERR_MEMORY_LIMIT:        return "Memory Limit";
    case SERDEC_ERR_OUT_OF_MEMORY:       return "Out of Memory";

    case SERDEC_ERR_IO:                  return "IO";
    case SERDEC_ERR_FILE_NOT_FOUND:      return "File Not Found";

    case SERDEC_ERR_INVALID_HANDLE:      return "Invalid Handle";

    default:                             return "Unknown Error";
    }
}

void serdec_error_format(const SerdecErrorInfo *info, char *buf, size_t bufsize) {
    if (!info || !buf || !bufsize) return;

    size_t pos = 0;
    pos += snprintf(buf, bufsize, "Error: %s\nAt: line %zu, column %zu (offset %zu)\n",
            serdec_error_string(info->code), info->line, info->column, info->offset);

    if (pos >= bufsize) return;

    if (info->path[0] != '\0')
        pos += snprintf(buf + pos, bufsize - pos, "Path: %s", info->path);
    
    if (pos >= bufsize) return;

    if (info->context[0] != '\0')
        pos += snprintf(buf + pos, bufsize - pos, "Context: %s", info->context);

    if (pos >= bufsize) return;
    
    if (info->message[0] != '\0')
        snprintf(buf + pos, bufsize - pos, "Message: %s", info->message);
}
