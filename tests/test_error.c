#include "test.h"
#include <serdec/serdec.h>

TEST(error_string_ok) {
    ASSERT(strcmp(serdec_error_string(SERDEC_OK), "OK") == 0);
}

TEST(error_string_codes) {
    // Syntax errors
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_UNEXPECTED_CHAR), "Unexpected") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_UNEXPECTED_EOF), "EOF") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_INVALID_VALUE), "Value") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_TRAILING_CHARS), "Trailing") != NULL);

    // String errors
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_INVALID_ESCAPE), "Escape") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_INVALID_UTF8), "UTF") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_UNTERMINATED_STRING), "String") != NULL);

    // Number errors
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_INVALID_NUMBER), "Number") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_NUMBER_OVERFLOW), "Overflow") != NULL);

    // Resource errors
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_DEPTH_LIMIT), "Depth") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_MEMORY_LIMIT), "Memory") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_OUT_OF_MEMORY), "Memory") != NULL);

    // I/O errors
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_IO), "IO") != NULL);
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_FILE_NOT_FOUND), "File") != NULL);

    // Handle error
    ASSERT(strstr(serdec_error_string(SERDEC_ERR_INVALID_HANDLE), "Handle") != NULL);
}

TEST(error_string_unknown) {
    const char* s = serdec_error_string((SerdecError)9999);
    ASSERT_NOT_NULL(s);
}

TEST(error_format_line_column) {
    SerdecErrorInfo info = {
        .code = SERDEC_ERR_INVALID_VALUE,
        .offset = 25,
        .line = 3,
        .column = 10
    };

    char buf[1024];
    serdec_error_format(&info, buf, sizeof(buf));

    ASSERT(strstr(buf, "3") != NULL);
    ASSERT(strstr(buf, "10") != NULL);
}

TEST(error_format_path) {
    SerdecErrorInfo info = {
        .code = SERDEC_ERR_INVALID_NUMBER,
        .line = 1,
        .column = 1
    };
    strcpy(info.path, "$.users[42].age");

    char buf[1024];
    serdec_error_format(&info, buf, sizeof(buf));

    ASSERT(strstr(buf, "$.users[42].age") != NULL);
}

TEST(error_format_empty_fields) {
    SerdecErrorInfo info = {
        .code = SERDEC_ERR_UNEXPECTED_EOF,
        .offset = 0,
        .line = 1,
        .column = 1,
        .path = "",
        .message = "",
        .context = ""
    };

    char buf[1024];
    serdec_error_format(&info, buf, sizeof(buf));

    ASSERT(strlen(buf) > 0);
}

TEST(error_format_no_overflow) {
    SerdecErrorInfo info = {
        .code = SERDEC_ERR_INVALID_VALUE,
        .line = 1,
        .column = 1
    };

    memset(info.path, 'x', sizeof(info.path) - 1);
    info.path[sizeof(info.path) - 1] = '\0';
    memset(info.message, 'y', sizeof(info.message) - 1);
    info.message[sizeof(info.message) - 1] = '\0';

    char buf[64];
    serdec_error_format(&info, buf, sizeof(buf));

    ASSERT(strlen(buf) < 64);
}

TEST(error_format_null_safety) {
    char buf[256];
    serdec_error_format(NULL, buf, sizeof(buf));

    SerdecErrorInfo info = { .code = SERDEC_OK };
    serdec_error_format(&info, NULL, 0);
}

int test_error(void) {
    printf("\n  Error tests:\n");

    RUN(error_string_ok);
    RUN(error_string_codes);
    RUN(error_string_unknown);
    RUN(error_format_line_column);
    RUN(error_format_path);
    RUN(error_format_empty_fields);
    RUN(error_format_no_overflow);
    RUN(error_format_null_safety);

    TEST_SUMMARY();
}
