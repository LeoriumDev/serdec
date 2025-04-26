/**
 * Serdec - A C serialization and deserialization library
 * Version: 1.0.0
 * Copyright (c) 2025 Leorium <contact@leorium.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 *
 * SPDX-License-Identifier: MIT
 */

#include "serdec_json.h"

struct serdec_json_list {
    struct serdec_json* head;
    struct serdec_json* tail;
    size_t length;
};

struct serdec_json {
    char* key;
    serdec_json_type json_type;
    union {
        void* null;
        bool bool_val;
        int64_t int_val;
        double float_val;
        char* str_val;
        serdec_json_list_t* children;
    } value;
    struct serdec_json* next;
};

serdec_json_t* serdec_json_new_node(void) {
    serdec_json_t* node = malloc(sizeof(serdec_json_t));
    if (!node)
        return NULL;

    node->key = NULL;
    node->json_type = SERDEC_JSON_NULL;
    memset(&node->value, 0, sizeof(node->value));
    node->next = NULL;
    return node;
}

serdec_json_t* serdec_json_new_null(void) {
    return serdec_json_new_node();
}

serdec_json_t* serdec_json_new_bool(bool value) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_BOOLEAN;
    node->value.bool_val = value;
    return node;
}

serdec_json_t* serdec_json_new_int(int64_t value) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_INT;
    node->value.int_val = value;
    return node;
}

serdec_json_t* serdec_json_new_float(double value) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_FLOAT;
    node->value.float_val = value;
    return node;
}

serdec_json_t* serdec_json_new_string(const char* string) {
    if (!string)
        return NULL;

    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_STRING;

    node->value.str_val = malloc(strlen(string) + 1);
    if (!node->value.str_val) {
        free(node);
        return NULL;
    }

    strcpy(node->value.str_val, string);
    return node;
}

serdec_json_t* serdec_json_new_array(void) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_ARRAY;

    node->value.children = malloc(sizeof(serdec_json_list_t));
    if (!node->value.children) {
        free(node);
        return NULL;
    }

    node->value.children->head = NULL;
    node->value.children->tail = NULL;
    node->value.children->length = 0;
    return node;
}

serdec_json_t* serdec_json_new_object(void) {
    serdec_json_t* node = serdec_json_new_node();
    if (!node)
        return NULL;

    node->json_type = SERDEC_JSON_OBJECT;

    node->value.children = malloc(sizeof(serdec_json_list_t));
    if (!node->value.children) {
        free(node);
        return NULL;
    }

    node->value.children->head = NULL;
    node->value.children->tail = NULL;
    node->value.children->length = 0;
    return node;
}

bool serdec_json_add_null(serdec_json_t* object, const char* key, void* null) {
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT ||
        !object->value.children)
        return false;

    (void) null;

    serdec_json_t* new_node = serdec_json_new_null();
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);

    return serdec_json_list_append(object, new_node);
}

bool serdec_json_add_bool(serdec_json_t* object, const char* key, bool value) {
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT||
        !object->value.children)
        return false;

    serdec_json_t* new_node = serdec_json_new_bool(value);
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);

    return serdec_json_list_append(object, new_node);  
}

bool serdec_json_add_int(serdec_json_t* object, const char* key, int64_t value) {
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT ||
        !object->value.children)
        return false;

    serdec_json_t* new_node = serdec_json_new_int(value);
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);
    new_node->next = NULL;
    
    return serdec_json_list_append(object, new_node);
}

bool serdec_json_add_float(serdec_json_t* object, const char* key, double value) {
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT ||
        !object->value.children)
        return false;

    serdec_json_t* new_node = serdec_json_new_float(value);
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);

    return serdec_json_list_append(object, new_node); 
}

bool serdec_json_add_string(serdec_json_t* object, const char* key, const char* value) {
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT ||
        !object->value.children)
        return false;

    serdec_json_t* new_node = serdec_json_new_string(value);
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);

    return serdec_json_list_append(object, new_node);
}

bool serdec_json_add_object(serdec_json_t* object, const char* key, serdec_json_t* value) {
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT ||
        !object->value.children)
        return false;

    serdec_json_t* new_node = serdec_json_new_object();
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);
    new_node->value.children = value->value.children;

    return serdec_json_list_append(object, new_node);    
}

bool serdec_json_add_array(serdec_json_t* object, const char* key, serdec_json_t* value) {
    if (!object ||  value->json_type != SERDEC_JSON_ARRAY)
        return false;

    serdec_json_t* new_node = serdec_json_new_array();
    if (!new_node)
        return false;

    new_node->key = malloc(strlen(key) + 1);
    if (!new_node->key) {
        free(new_node);
        return false;
    }

    strcpy(new_node->key, key);
    new_node->value.children = value->value.children;

    return serdec_json_list_append(object, new_node); 
}

bool serdec_json_list_append(serdec_json_t* object, serdec_json_t* new_node) {
    if (!object || !object->value.children || !new_node)
        return false;

    serdec_json_list_t* list = object->value.children;
    if (!list->head) {
        list->head = list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->length++;
    return true;
}

bool serdec_json_array_append(serdec_json_t* object, serdec_json_t* element) {
    if (!object || !object->value.children || !element)
        return false;
    return serdec_json_list_append(object, element);
}

char* serdec_json_stringify(serdec_json_t* object) {
    if (!object)
        return NULL;

    char* string = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    if (!string)
        return NULL;
    string[0] = '\0';

    char tmp[4096];
    bool need_comma = false;

    if (object->json_type == SERDEC_JSON_OBJECT) {
        strcat(string, "{\n");

        serdec_json_t* ptr = object->value.children->head;
        while (ptr) {
            if (need_comma)
                strcat(string, ",\n");

            strcat(string, SERDEC_INDENT);
            strcat(string, "\"");
            strcat(string, ptr->key);
            strcat(string, "\": ");

            if (ptr->json_type == SERDEC_JSON_OBJECT) {
                strcat(string, "{\n");

                serdec_json_t* child = ptr->value.children->head;
                bool first_child = true;
                while (child) {
                    if (!first_child)
                        strcat(string, ",\n");
                    strcat(string, SERDEC_INDENT);
                    strcat(string, SERDEC_INDENT);
                    strcat(string, "\"");
                    strcat(string, child->key);
                    strcat(string, "\": ");

                    char* nested = serdec_json_stringify(child);
                    strcat(string, nested);
                    free(nested);

                    first_child = false;
                    child = child->next;
                }
                strcat(string, "\n");
                strcat(string, SERDEC_INDENT);
                strcat(string, "}");
            } else if (ptr->json_type == SERDEC_JSON_ARRAY) {
                strcat(string, "[\n");

                serdec_json_t* element = ptr->value.children->head;
                bool first_element = true;
                while (element) {
                    if (!first_element)
                        strcat(string, ",");
                    strcat(string, SERDEC_INDENT);
                    strcat(string, SERDEC_INDENT);

                    char* nested = serdec_json_stringify(element);
                    strcat(string, nested);
                    free(nested);

                    first_element = false;
                    element = element->next;
                }
                strcat(string, "\n");
                strcat(string, SERDEC_INDENT);
                strcat(string, "]");
            } else {
                switch (ptr->json_type) {
                    case SERDEC_JSON_NULL:
                        strcat(string, "null");
                        break;
                    case SERDEC_JSON_BOOLEAN:
                        strcat(string, (ptr->value.bool_val) ? "true" : "false");
                        break;
                    case SERDEC_JSON_INT:
                        snprintf(tmp, sizeof(tmp), "%" PRId64, ptr->value.int_val);
                        strcat(string, tmp);
                        break;
                    case SERDEC_JSON_FLOAT:
                        snprintf(tmp, sizeof(tmp), "%f", ptr->value.float_val);
                        strcat(string, tmp);
                        break;
                    case SERDEC_JSON_STRING:
                        strcat(string, "\"");
                        strcat(string, ptr->value.str_val);
                        strcat(string, "\"");
                        break;
                    default:
                        strcat(string, "null");
                        break;
                }
            }

            need_comma = true;
            ptr = ptr->next;
        }

        strcat(string, "}");
    } else if (object->json_type == SERDEC_JSON_ARRAY) {
        strcat(string, "[");

        serdec_json_t* ptr = object->value.children->head;
        bool first = true;
        while (ptr) {
            if (!first)
                strcat(string, ",");

            strcat(string, SERDEC_INDENT);

            char* nested = serdec_json_stringify(ptr);
            strcat(string, nested);
            free(nested);

            first = false;
            ptr = ptr->next;
        }

        strcat(string, "]");
    } else {
        // Primitive node
        switch (object->json_type) {
            case SERDEC_JSON_NULL:
                strcat(string, "null");
                break;
            case SERDEC_JSON_BOOLEAN:
                strcat(string, (object->value.bool_val) ? "true" : "false");
                break;
            case SERDEC_JSON_INT:
                snprintf(tmp, sizeof(tmp), "%" PRId64, object->value.int_val);
                strcat(string, tmp);
                break;
            case SERDEC_JSON_FLOAT:
                snprintf(tmp, sizeof(tmp), "%f", object->value.float_val);
                strcat(string, tmp);
                break;
            case SERDEC_JSON_STRING:
                strcat(string, "\"");
                strcat(string, object->value.str_val);
                strcat(string, "\"");
                break;
            default:
                strcat(string, "null");
                break;
        }
    }

    return string;
}

serdec_json_t* serdec_json_parse(const char* json_string) {
    return NULL;
}

void serdec_json_free(serdec_json_t* node) {
    if (!node)
        return;

    if (node->key)
        free(node->key);

    switch (node->json_type) {
        case SERDEC_JSON_OBJECT:
        case SERDEC_JSON_ARRAY: {
            serdec_json_list_t* list = node->value.children;
            if (list) {
                serdec_json_t* current = list->head;
                while (current) {
                    serdec_json_t* next = current->next;
                    serdec_json_free(current);
                    current = next;
                }
                free(list);
            }
            break;
        }
        case SERDEC_JSON_STRING:
            if (node->value.str_val)
                free(node->value.str_val);
            break;
        default:
            break;
    }

    free(node);
}