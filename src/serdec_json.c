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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "serdec_json.h"
#include "serdec_arena.h"
#include "serdec_vector.h"
#include "serdec_string.h"


/* json field types */
typedef enum {
    SERDEC_JSON_NULL    ,
    SERDEC_JSON_BOOLEAN ,
    SERDEC_JSON_INT     ,
    SERDEC_JSON_FLOAT   ,
    SERDEC_JSON_STRING  ,
    SERDEC_JSON_ARRAY   ,
    SERDEC_JSON_OBJECT
} serdec_json_type;

/* json type */
struct serdec_json {
    serdec_string_t*    key;
    serdec_json_type    type;
    union {
        bool             boolean;
        int64_t          i64;
        double           f64;
        serdec_string_t* string;
        serdec_vector_t* vector;
    } value;
    serdec_arena_t*     arena;
};

static inline void serdec_json_init_node(serdec_json_t* parent,
                                         serdec_json_t* node,
                                         serdec_json_type type,
                                         const char* key) {
    memset(&node->value, 0, sizeof(node->value));
    node->type = type;
    node->arena = parent->arena;
    node->key = serdec_string_new(key, parent->arena);
}

static inline void serdec_add_indent(serdec_string_t* string, size_t indent_level) {
    while (indent_level--)
        serdec_string_push(string, SERDEC_INDENT);
}

serdec_json_t* serdec_json_new_root(void) {
    serdec_arena_t* arena = serdec_arena_create(4096);
    if (!arena)
        return NULL;
        
    serdec_json_t* root =
        SERDEC_ARENA_ALLOC(arena, serdec_json_t);
    if (!root)
        return NULL;
        
    memset(&root->value, 0, sizeof(root->value));
    root->type = SERDEC_JSON_OBJECT;
    root->arena = arena;
    
    root->key = serdec_string_new("", arena);
    if (!root->key)
        return NULL;
        
    root->value.vector = serdec_vector_new(sizeof(serdec_json_t*), 
                                         SERDEC_INITIAL_OBJECT_CAP, 
                                         arena);
    if (!root->value.vector)
        return NULL;
        
    return root;
}


serdec_json_t* serdec_json_new_null(serdec_json_t* root, const char* key) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_NULL, key);
    return node;
}

serdec_json_t* serdec_json_new_bool(serdec_json_t* root, const char* key, bool value) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_BOOLEAN, key);
    node->value.boolean = value;
    return node;
}

serdec_json_t* serdec_json_new_int(serdec_json_t* root, const char* key, int64_t value) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_INT, key);
    node->value.i64 = value;
    return node;
}

serdec_json_t* serdec_json_new_float(serdec_json_t* root, const char* key, double value) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_FLOAT, key);
    node->value.f64 = value;
    return node;
}

serdec_json_t* serdec_json_new_string(serdec_json_t* root, const char* key, const char* string) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_STRING, key);
    node->value.string = serdec_string_new(string, root->arena);
    return node;
}

serdec_json_t* serdec_json_new_array(serdec_json_t* root, const char* key) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_ARRAY, key);
    node->value.vector = serdec_vector_new(sizeof(serdec_json_t),
                                           SERDEC_INITIAL_ARRAY_CAP,
                                           root->arena);
    return node;
}

serdec_json_t* serdec_json_new_object(serdec_json_t* root, const char* key) {
    serdec_json_t* node = SERDEC_ARENA_ALLOC(root->arena, serdec_json_t);
    serdec_json_init_node(root, node, SERDEC_JSON_OBJECT, key);
    node->value.vector = serdec_vector_new(sizeof(serdec_json_t),
                                           SERDEC_INITIAL_OBJECT_CAP,
                                           root->arena);
    return node;
}

bool serdec_json_add_null(serdec_json_t* object, const char* key, void* null) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;

    (void) null;

    serdec_json_t* new_node = serdec_json_new_null(object, key);
    serdec_vector_push(object->value.vector, &new_node);
    return true;
}

bool serdec_json_add_bool(serdec_json_t* object, const char* key, bool value) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;

    serdec_json_t* new_node = serdec_json_new_bool(object, key, value);
    serdec_vector_push(object->value.vector, &new_node);
    return true;
}

bool serdec_json_add_int(serdec_json_t* object, const char* key,
                         int64_t value) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;

    serdec_json_t* new_node = serdec_json_new_int(object, key, value);
    serdec_vector_push(object->value.vector, &new_node);
    return true;
}

bool serdec_json_add_float(serdec_json_t* object, const char* key,
                           double value) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;

    serdec_json_t* new_node = serdec_json_new_float(object, key, value);
    serdec_vector_push(object->value.vector, &new_node);
    return true;
}

bool serdec_json_add_string(serdec_json_t* object, const char* key,
                            const char* value) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;

    serdec_json_t* new_node = serdec_json_new_string(object, key, value);
    serdec_vector_push(object->value.vector, &new_node);
    return true;
}

bool serdec_json_add_object(serdec_json_t* object, const char* key,
                            serdec_json_t* value) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;
                                
    serdec_vector_push(object->value.vector, &value);
    return true;
}

bool serdec_json_add_array(serdec_json_t* object, const char* key,
                           serdec_json_t* value) {
    if (!object || !key || object->type != SERDEC_JSON_OBJECT)
        return false;

    serdec_vector_push(object->value.vector, &value);
    return true;
}

char* serdec_json_stringify_internal(serdec_json_t* object, bool pretty_print) {
    if (!object)
        return NULL;
    serdec_string_t* str = serdec_string_new(NULL, object->arena);

    serdec_vector_t* arr = object->value.vector;
    if (!arr)
        return NULL;

    size_t indent_level = 0;
    serdec_string_push(str, pretty_print ? "{\n" : "{");
    for (size_t i = 0; i < arr->count; ++i) {
        serdec_json_t** data_ptr = (serdec_json_t**)arr->data + i;
        serdec_json_t* data = *data_ptr;
        switch (data->type) {
        case SERDEC_JSON_NULL:
        case SERDEC_JSON_INT:
        case SERDEC_JSON_BOOLEAN:
        case SERDEC_JSON_FLOAT:
        case SERDEC_JSON_STRING:
            serdec_string_push(str, serdec_json_stringify_primitive(
                data, indent_level + 1, pretty_print));
            break;
        case SERDEC_JSON_ARRAY:
            serdec_string_push(str, serdec_json_stringify_array(
                data, indent_level + 1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            serdec_string_push(str, serdec_json_stringify_object(
                data, indent_level + 1, pretty_print));
            break;
        default:
            serdec_string_push(str, "(ERROR)");
            break;
        }
        if (pretty_print)
            serdec_string_push(str, (i < arr->count - 1) ? ",\n" : "\n");
        else
            serdec_string_push(str, (i < arr->count - 1) ? "," : "");
    }
    serdec_string_push(str, "}");
    char* output = serdec_string_take(str);
    return output;
}

char* serdec_json_stringify(serdec_json_t* object) {
    return serdec_json_stringify_internal(object, true);
}

char* serdec_json_stringify_raw(serdec_json_t* object) {
    return serdec_json_stringify_internal(object, false);
}

char* serdec_json_stringify_primitive(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object ||
        (object->type != SERDEC_JSON_NULL &&
        object->type != SERDEC_JSON_BOOLEAN &&
        object->type != SERDEC_JSON_INT &&
        object->type != SERDEC_JSON_FLOAT &&
        object->type != SERDEC_JSON_STRING))
        return NULL;

    serdec_string_t* str = serdec_string_new(NULL, object->arena);

    if (pretty_print)
        serdec_add_indent(str, indent_level);

    if (object->key)
        serdec_string_printf(str, "\"%s\":%s", (char*)object->key->vec->data, pretty_print ? " " : "");

    switch (object->type) {
    case SERDEC_JSON_NULL:
        serdec_string_push(str, "null");
        break;
    case SERDEC_JSON_BOOLEAN:
        serdec_string_push(str, object->value.boolean ? "true" : "false");
        break;
    case SERDEC_JSON_INT:
        serdec_string_printf(str, "%" PRId64, object->value.i64);
        break;
    case SERDEC_JSON_FLOAT:
        serdec_string_printf(str, "%lf", object->value.f64);
        break;
    case SERDEC_JSON_STRING:
        serdec_string_printf(str, "\"%s\"", (char*)object->value.string->vec->data);
        break;
    default:
        serdec_string_push(str, "(ERROR)");
        break;
    }
    char* output = serdec_string_take(str);
    return output;
}

char* serdec_json_stringify_array(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object || object->type != SERDEC_JSON_ARRAY)
        return NULL;
    
    serdec_string_t* str = serdec_string_new(NULL, object->arena);

    if (pretty_print)
        serdec_add_indent(str, indent_level);

    if (object->key)
        serdec_string_printf(str, "\"%s\":%s", (char*)object->key->vec->data, pretty_print ? " " : "");
    
    serdec_string_push(str, pretty_print ? "[\n" : "[");

    serdec_vector_t* arr = object->value.vector;
    for (size_t i = 0; i < arr->count; ++i) {
        serdec_json_t* element = ((serdec_json_t**)arr->data)[i];
        switch (element->type) {
        case SERDEC_JSON_ARRAY:
            serdec_string_push(str, serdec_json_stringify_array(
                element, indent_level+1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            serdec_string_push(str, serdec_json_stringify_object(
                element, indent_level+1, pretty_print));
            break;
        default:
            serdec_string_push(str, serdec_json_stringify_primitive(
                element, indent_level+1, pretty_print));
            break;
        }
        if (pretty_print)
            serdec_string_push(str, (i < arr->count - 1) ? ",\n" : "\n");
        else
            serdec_string_push(str, (i < arr->count - 1) ? "," : "");
    }
    
    if (pretty_print)
        serdec_add_indent(str, indent_level);
        
    serdec_string_push(str, "]");
    char* output = serdec_string_take(str);
    return output;
}

char* serdec_json_stringify_object(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object || object->type != SERDEC_JSON_OBJECT)
        return NULL;

    serdec_string_t* str = serdec_string_new(NULL, object->arena);
    if (!str)
        return NULL;

    if (pretty_print)
        serdec_add_indent(str, indent_level);

    if (object->key)
        serdec_string_printf(str, "\"%s\":%s", (char*)object->key->vec->data, pretty_print ? " " : "");

    serdec_string_push(str, pretty_print ? "{\n" : "{");

    serdec_vector_t* arr = object->value.vector;
    for (size_t i = 0; i < arr->count; ++i) {
        serdec_json_t* element = ((serdec_json_t**)arr->data)[i];
        switch (element->type) {
        case SERDEC_JSON_ARRAY:
            serdec_string_push(str, serdec_json_stringify_array(
                element, indent_level+1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            serdec_string_push(str, serdec_json_stringify_object(
                element, indent_level+1, pretty_print));
            break;
        default:
            serdec_string_push(str, serdec_json_stringify_primitive(
                element, indent_level+1, pretty_print));
            break;
        }
        if (pretty_print)
            serdec_string_push(str, (i < arr->count - 1) ? ",\n" : "\n");
        else
            serdec_string_push(str, (i < arr->count - 1) ? "," : "");
    }

    if (pretty_print)
        serdec_add_indent(str, indent_level);
    serdec_string_push(str, "}");

    return serdec_string_take(str);
}

void serdec_json_free(serdec_json_t* root) {
    if (!root)
        return;

    serdec_arena_destroy(root->arena);
}
