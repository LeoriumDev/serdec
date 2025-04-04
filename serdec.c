#include "serdec.h"

JSON* json_create_object(void) {
    JSON* object = malloc(sizeof(JSON));
    object->next = object;
    object->prev = object;
    object->child = NULL;
    return object;
}