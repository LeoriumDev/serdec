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

enum serdec_json_type {
    SERDEC_JSON_NULL    ,
    SERDEC_JSON_BOOLEAN ,
    SERDEC_JSON_INT     ,
    SERDEC_JSON_FLOAT   ,
    SERDEC_JSON_STRING  ,
    SERDEC_JSON_ARRAY   ,
    SERDEC_JSON_OBJECT
};

struct serdec_json {
    char* key;
    serdec_json_type json_type;
    union {
        bool bool_val;
        int64_t int_val;
        double float_val;
        char* str_val;
        serdec_json_list_t* children;
    } value;
    serdec_json_t* next;
};

struct serdec_json_list {
    serdec_json_t* head;
    serdec_json_t* tail;
    size_t length;
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

serdec_json_t* serdec_json_new_null(void* null) {
    (void) null;
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

    serdec_json_t* new_node = serdec_json_new_null(NULL);
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
    if (!object || !key || object->json_type != SERDEC_JSON_OBJECT ||
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

bool serdec_json_add_int(serdec_json_t* object, const char* key,
                         int64_t value) {
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

bool serdec_json_add_float(serdec_json_t* object, const char* key,
                           double value) {
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

bool serdec_json_add_string(serdec_json_t* object, const char* key,
                            const char* value) {
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

bool serdec_json_add_object(serdec_json_t* object, const char* key,
                            serdec_json_t* value) {
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

bool serdec_json_add_array(serdec_json_t* object, const char* key,
                           serdec_json_t* value) {
    if (!object || !key || value->json_type != SERDEC_JSON_ARRAY)
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

char* serdec_json_stringify_internal(serdec_json_t* object, bool pretty_print) {
    if (!object)
        return NULL;

    char* buffer = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    char* tmp = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    if (!buffer || !tmp)
        return NULL;

    buffer[0] = '\0';
    tmp[0] = '\0';

    serdec_json_t* data = object->value.children->head;
    size_t indent_level = 0;

    if (!data)
        return NULL;

    strcpy(buffer, pretty_print ? "{\n" : "{");
    while (data) {
        switch (data->json_type) {
        case SERDEC_JSON_NULL:
        case SERDEC_JSON_INT:
        case SERDEC_JSON_BOOLEAN:
        case SERDEC_JSON_FLOAT:
        case SERDEC_JSON_STRING:
            strcat(buffer, serdec_json_stringify_primitive(data, indent_level + 1, pretty_print));
            break;
        case SERDEC_JSON_ARRAY:
            strcat(buffer, serdec_json_stringify_array(data, indent_level + 1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            strcat(buffer, serdec_json_stringify_object(data, indent_level + 1, pretty_print));
            break;
        default:
            break;
        }
        if (pretty_print)
            strcat(buffer, data->next ? ",\n" : "\n");
        else
            strcat(buffer, data->next ? "," : "");
        data = data->next;
    }
    strcat(buffer, "}");
    return buffer;
}

char* serdec_json_stringify(serdec_json_t* object) {
    return serdec_json_stringify_internal(object, true);
}

char* serdec_json_stringify_raw(serdec_json_t* object) {
    return serdec_json_stringify_internal(object, false);
}

char* serdec_json_stringify_primitive(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object ||
        (object->json_type != SERDEC_JSON_NULL &&
        object->json_type != SERDEC_JSON_BOOLEAN &&
        object->json_type != SERDEC_JSON_INT &&
        object->json_type != SERDEC_JSON_FLOAT &&
        object->json_type != SERDEC_JSON_STRING))
        return NULL;
    char* buffer = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    char* tmp = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    if (!buffer || !tmp)
        return NULL;

    buffer[0] = '\0';
    tmp[0] = '\0';
    if (pretty_print)
        serdec_add_indent(buffer, indent_level);

    if (object->key) {
        strcat(buffer, "\"");
        strcat(buffer, object->key);
        strcat(buffer, pretty_print ? "\": " : "\":");
    }    
    switch (object->json_type) {
    case SERDEC_JSON_NULL:
        strcat(buffer, "null");
        break;
    case SERDEC_JSON_BOOLEAN:
        strcat(buffer, object->value.bool_val ? "true" : "false");
        break;
    case SERDEC_JSON_INT:
        sprintf(tmp, "%" PRId64, object->value.int_val);
        strcat(buffer, tmp);
        break;
    case SERDEC_JSON_FLOAT:
        sprintf(tmp, "%lf", object->value.float_val);
        strcat(buffer, tmp);
        break;
    case SERDEC_JSON_STRING:
        strcat(buffer, "\"");
        strcat(buffer, object->value.str_val);
        strcat(buffer, "\"");
        break;
    default:
        strcat(buffer, "\"\"");
        break;
    }
    return buffer;
}

char* serdec_json_stringify_array(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object || object->json_type != SERDEC_JSON_ARRAY)
        return NULL;
    
    char* buffer = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    char* tmp = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    if (!buffer || !tmp)
        return NULL;

    buffer[0] = '\0';
    tmp[0] = '\0';
    if (pretty_print)
        serdec_add_indent(buffer, indent_level);

    if (object->key) {
        strcat(buffer, "\"");
        strcat(buffer, object->key);
        strcat(buffer, "\": ");
    }
    
    strcat(buffer, pretty_print ? "[\n" : "[");

    serdec_json_t* element = object->value.children->head;
    while (element) {
        if (element->json_type == SERDEC_JSON_ARRAY)
            strcat(buffer, serdec_json_stringify_array(element, indent_level+1, pretty_print));
        else if (element->json_type == SERDEC_JSON_OBJECT)
            strcat(buffer, serdec_json_stringify_object(element, indent_level+1, pretty_print));
        else
            strcat(buffer, serdec_json_stringify_primitive(element, indent_level+1, pretty_print));

        if (pretty_print)
            strcat(buffer, element->next ? ",\n" : "\n");
        else
            strcat(buffer, element->next ? "," : "");
        element = element->next;
    }
    if (pretty_print)
        serdec_add_indent(buffer, indent_level);
    strcat(buffer, "]");
    return buffer;
}

char* serdec_json_stringify_object(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object || object->json_type != SERDEC_JSON_OBJECT)
        return NULL;
    
    char* buffer = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    char* tmp = malloc(SERDEC_INITIAL_BUFFER_SIZE);
    if (!buffer || !tmp)
        return NULL;

    buffer[0] = '\0';
    tmp[0] = '\0';
    if (pretty_print)
        serdec_add_indent(buffer, indent_level);
    if (object->key) {
        strcat(buffer, "\"");
        strcat(buffer, object->key);
        strcat(buffer, pretty_print ? "\": {\n" : "\":{");
    } else {
        strcat(buffer, pretty_print ? "{\n" : "{");
    }

    serdec_json_t* element = object->value.children->head;
    while (element) {
        if (element->json_type == SERDEC_JSON_ARRAY)
            strcat(buffer, serdec_json_stringify_array(element, indent_level+1, pretty_print));
        else if (element->json_type == SERDEC_JSON_OBJECT)
            strcat(buffer, serdec_json_stringify_object(element, indent_level+1, pretty_print));
        else
            strcat(buffer, serdec_json_stringify_primitive(element, indent_level+1, pretty_print));

        if (pretty_print)
            strcat(buffer, element->next ? ",\n" :"\n");
        else
            strcat(buffer, element->next ? "," :"");
        element = element->next;
    }
    if (pretty_print)
        serdec_add_indent(buffer, indent_level);
    strcat(buffer, "}");
    return buffer;
}

void serdec_json_free(serdec_json_t* node) {
    if (!node) return;

    free(node->key);
    node->key = NULL;

    switch (node->json_type) {
        case SERDEC_JSON_OBJECT:
        case SERDEC_JSON_ARRAY: {
            serdec_json_list_t* list = node->value.children;
            if (list) {
                serdec_json_t* current = list->head;
                while (current) {
                    current->next = NULL;
                    serdec_json_free(current);
                    current = current->next;
                }
                free(list);
                node->value.children = NULL;
            }
            break;
        }
        case SERDEC_JSON_STRING:
            free(node->value.str_val);
            node->value.str_val = NULL;
            break;
        default:
            break;
    }

    free(node);
}

void serdec_add_indent(char* buffer, size_t indent_level) {
    if (!buffer)
        return;
    while (indent_level--)
        strcat(buffer, SERDEC_INDENT);
}

serdec_json_t* serdec_json_object_return(serdec_json_t* object) {
    return object;
}
