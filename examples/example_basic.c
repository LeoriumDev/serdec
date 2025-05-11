#include <stdio.h>
#include <stdlib.h>
#include "serdec_json.h"

int main(void) {
    serdec_json_t* root = serdec_json_new_root();
    serdec_json_add_int(root, "test", 123);
    char* result = serdec_json_stringify(root);
    printf("%s\n", result);
    serdec_json_free(root);
    return EXIT_SUCCESS;
}
