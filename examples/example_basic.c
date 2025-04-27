#include <stdio.h>
#include "serdec_json.h"

int main(void) {
    serdec_json_t* obj = serdec_json_new_object();
    serdec_json_add_string(obj, "name", "Serdec");
    serdec_json_add_int(obj, "year", 2025);

    char* json = serdec_json_stringify(obj);
    printf("%s\n", json);

    free(json);
    serdec_json_free(obj);

    return 0;
}