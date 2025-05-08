#include <stdio.h>
#include "serdec_json.h"

int main(void) {
    serdec_json_t* root = serdec_json_new_object();
    serdec_json_add_string(root, "name", "Leorium");
    serdec_json_add_int(root, "age", 20);
    serdec_json_t* obj = serdec_json_new_object();
    serdec_json_add_string(obj, "hello", "world");
    serdec_json_t* hobbies = serdec_json_new_array();
    serdec_json_t* test = serdec_json_new_array();
    serdec_json_array_append(hobbies, serdec_json_new_string("C Programming"));
    serdec_json_array_append(hobbies, serdec_json_new_int(123));
    serdec_json_array_append(test, serdec_json_new_int(1));
    serdec_json_array_append(test, serdec_json_new_bool(true));
    serdec_json_add_array(root, "hobbies", hobbies);
    serdec_json_add_array(root, "testing", test);
    serdec_json_array_append(hobbies, obj);
    char* output = serdec_json_stringify(root);
    printf("%s\n", output);

    serdec_json_free(root);
    free(output);

    return EXIT_SUCCESS;
}
