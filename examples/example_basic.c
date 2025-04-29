#include <stdio.h>
#include "serdec_json.h"

int main(void) {
    // Create a root object
    serdec_json_t* root = serdec_json_new_object();
    // Add various types using direct functions
    serdec_json_add_null(root, "null_direct", NULL);
    serdec_json_add_bool(root, "bool_direct", true);
    serdec_json_add_int(root, "int_direct", 123);
    serdec_json_add_float(root, "float_direct", 3.1415);
    serdec_json_add_string(root, "string_direct", "Hello, Serdec!");

    // Create an array
    serdec_json_t* array = serdec_json_new_array();

    // Push elements into the array using generic macro
    serdec_json_array_push(array, NULL);          // null
    serdec_json_array_push(array, true);         // boolean
    serdec_json_array_push(array, 456);          // int
    serdec_json_array_push(array, 7.89);         // float
    serdec_json_array_push(array, "Array String"); // string

    // Add the array into the object
    serdec_json_add_array(root, "array_direct", array);

    // Add a nested object
    serdec_json_t* nested = serdec_json_new_object();
    serdec_json_add_array(nested, "array", array);
    sjson_add_string(nested, "nested_key", "nested_value"); // using sjson_* alias
    serdec_json_add_object(root, "object_direct", nested);
    // Now use the generic macro to add various fields
    serdec_json_add(root, "null_macro", NULL);
    serdec_json_add(root, "bool_macro", false);
    serdec_json_add(root, "int_macro", 789);
    serdec_json_add(root, "float_macro", 6.28);
    serdec_json_add(root, "string_macro", "Macro String");

    // Serialize the whole JSON
    char* json_string = serdec_json_stringify(root);
    if (json_string) {
        printf("%s\n", json_string);
        free(json_string);
    }

    // Clean up
    serdec_json_free(root);

    return 0;
}
