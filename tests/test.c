#include <stdio.h>
#include <stdlib.h>

#include "serdec_json.h"

void test_stringify_basic();
void test_stringify_nested();
void test_stringify_empty();

void test_stringify_basic() {
    // Create a basic JSON object with different types
    serdec_json_t* root = serdec_json_new_root();
    serdec_json_add_int(root, "number", 123);
    serdec_json_add_bool(root, "boolean", true);
    serdec_json_add_float(root, "decimal", 45.67);
    serdec_json_add_string(root, "text", "Hello World");
    
    // Convert to string
    char* json_str = serdec_json_stringify(root);
    
    // Write the JSON to a file
    FILE* file = fopen("test.json", "w");
    if (file) {
        fprintf(file, "%s\n", json_str);
        fclose(file);
        printf("Basic stringify test: JSON written to build/test_basic.json\n");
    } else {
        printf("Error: Could not open file for basic test\n");
    }
    
    // Clean up
    serdec_json_free(root);
}

void test_stringify_nested() {
    // Create a JSON object with nested structure
    serdec_json_t* root = serdec_json_new_root();
    serdec_json_add(root, "id", 1);
    
    // Add nested object
    serdec_json_t* child = serdec_json_new_object(root, "testing");
    serdec_json_add_string(child, "name", "test");
    serdec_json_add_bool(child, "active", false);
    serdec_json_add_object(root, "metadata", child);
    
    // Add array
    serdec_json_t* array = serdec_json_new_array(root, "values");
    serdec_json_array_add(array, 1);
    serdec_json_array_add(array, 2);
    serdec_json_array_add(array, 3);
    serdec_json_add_array(root, "values", array);
    
    // Convert to string
    char* json_str = serdec_json_stringify(root);
    
    // Write the JSON to a file
    FILE* file = fopen("test_nested.json", "w");
    if (file) {
        fprintf(file, "%s\n", json_str);
        fclose(file);
        printf("Nested stringify test: JSON written to build/test_nested.json\n");
    } else {
        printf("Error: Could not open file for nested test\n");
    }
    
    // Clean up
    serdec_json_free(root);
}

void test_stringify_empty() {
    // Test empty object
    serdec_json_t* empty = serdec_json_new_root();
    char* json_str = serdec_json_stringify(empty);
    
    // Write the JSON to a file
    FILE* file = fopen("test_empty.json", "w");
    if (file) {
        fprintf(file, "%s\n", json_str);
        fclose(file);
        printf("Empty object stringify test: JSON written to build/test_empty.json\n");
    } else {
        printf("Error: Could not open file for empty test\n");
    }
    
    // Clean up
    serdec_json_free(empty);
}

int main(void) {
    printf("Running stringify tests...\n");
    
    test_stringify_basic();
    test_stringify_nested();
    test_stringify_empty();
    
    printf("All stringify tests passed!\n");
    return EXIT_SUCCESS;
}
