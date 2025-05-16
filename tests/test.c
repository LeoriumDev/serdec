#include <stdio.h>
#include <stdlib.h>

#include "serdec_json.h"

int main(void) {
    serdec_json_t* root = serdec_json_new_root();
    serdec_json_add_int(root, "test", 123);
    char* output = serdec_json_stringify(root);
    
    FILE* file = fopen("test.json", "w");
    fprintf(file, "%s", output);
    fclose(file);
    return EXIT_SUCCESS;
}
