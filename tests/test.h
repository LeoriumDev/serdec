#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>

// Colors
#define RED(s)    "\033[1;31m" s "\033[0m"
#define GREEN(s)  "\033[1;32m" s "\033[0m"
#define YELLOW(s) "\033[1;33m" s "\033[0m"
#define CYAN(s)   "\033[1;36m" s "\033[0m"

// Test state
static int _test_total;
static int _test_passed;
static int _test_failed;
static jmp_buf _test_jmp;
static int _test_did_fail;

#define TEST(name) static void test_##name(void)

#define RUN(name) do {                                                         \
    _test_total++;                                                             \
    _test_did_fail = 0;                                                        \
    {                                                                          \
        printf("    " CYAN("%s") " ", #name);                                  \
        int _pad = 50 - (int)strlen(#name);                                    \
        for (int _i = 0; _i < _pad; _i++) printf(".");                         \
    }                                                                          \
    if (setjmp(_test_jmp) == 0) {                                              \
        test_##name();                                                         \
        if (!_test_did_fail) {                                                 \
            _test_passed++;                                                    \
            printf(GREEN(" PASS") "\n");                                       \
        }                                                                      \
    } else {                                                                   \
        _test_failed++;                                                        \
    }                                                                          \
} while(0)

#define ASSERT(expr) do {                                                      \
    if (!(expr)) {                                                             \
        printf(RED(" FAIL") "\n");                                             \
        printf("      \033[1;31massertion failed: %s\033[0m\n", #expr);        \
        printf("      " YELLOW("%s:%d") "\n", __FILE__, __LINE__);             \
        _test_did_fail = 1;                                                    \
        longjmp(_test_jmp, 1);                                                 \
    }                                                                          \
} while(0)

#define ASSERT_EQ(a, b) do {                                                   \
    long long _a = (long long)(a);                                             \
    long long _b = (long long)(b);                                             \
    if (_a != _b) {                                                            \
        printf(RED(" FAIL") "\n");                                             \
        printf("      " RED("expected %lld, got %lld") "\n", _b, _a);         \
        printf("      " YELLOW("%s:%d") "\n", __FILE__, __LINE__);             \
        _test_did_fail = 1;                                                    \
        longjmp(_test_jmp, 1);                                                 \
    }                                                                          \
} while(0)

#define ASSERT_NULL(expr)     ASSERT((expr) == NULL)
#define ASSERT_NOT_NULL(expr) ASSERT((expr) != NULL)

#define TEST_SUMMARY() do {                                                    \
    printf("\n  ---------------------\n");                                      \
    printf("  Total: %d | ", _test_total);                                     \
    printf(GREEN("Passed: %d") " | ", _test_passed);                           \
    if (_test_failed > 0)                                                      \
        printf(RED("Failed: %d"), _test_failed);                               \
    else                                                                       \
        printf("Failed: 0");                                                   \
    printf("\n\n");                                                            \
    return _test_failed > 0 ? 1 : 0;                                          \
} while(0)
