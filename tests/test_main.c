#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int subtask1(void) {
    printf("[ RUN ] SubTask 1\n");
    sleep(1);
    printf("[ OK  ] SubTask 1\n");
    return 0;
}

static int subtask2(void) {
    printf("[ RUN ] SubTask 2\n");
    sleep(2);
    printf("[ OK  ] SubTask 2\n");
    return 0;
}

static int run_all(void) {
    int fail = 0;
    fail |= subtask1();
    fail |= subtask2();
    return fail;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        return run_all();
    }

    const char *name = argv[1];
    if (strcmp(name, "subtask1") == 0) return subtask1();
    if (strcmp(name, "subtask2") == 0) return subtask2();
    if (strcmp(name, "all") == 0)      return run_all();

    fprintf(stderr, "Unknown test: %s\n", name);
    fprintf(stderr, "Usage: %s [all|subtask1|subtask2]\n", argv[0]);
    return 2;
}
