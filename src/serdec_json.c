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

/* JSON field types */
typedef enum {
    SERDEC_JSON_NULL    ,
    SERDEC_JSON_BOOLEAN ,
    SERDEC_JSON_INT     ,
    SERDEC_JSON_FLOAT   ,
    SERDEC_JSON_STRING  ,
    SERDEC_JSON_ARRAY   ,
    SERDEC_JSON_OBJECT
} serdec_json_type;

struct serdec_buffer {
    char* data;
    size_t length;
    size_t capacity;
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

    serdec_buffer_t* buf = serdec_buffer_new(SERDEC_DEFAULT_BUFFER_CAP);
    if (!buf) 
        return NULL;

    serdec_json_t* data = object->value.children->head;
    if (!data)
        return NULL;

    size_t indent_level = 0;
    serdec_buffer_append_str(buf, pretty_print ? "{\n" : "{");
    while (data) {
        switch (data->json_type) {
        case SERDEC_JSON_NULL:
        case SERDEC_JSON_INT:
        case SERDEC_JSON_BOOLEAN:
        case SERDEC_JSON_FLOAT:
        case SERDEC_JSON_STRING:
            serdec_buffer_append_str(buf, serdec_json_stringify_primitive(
                data, indent_level + 1, pretty_print));
            break;
        case SERDEC_JSON_ARRAY:
            serdec_buffer_append_str(buf, serdec_json_stringify_array(
                data, indent_level + 1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            serdec_buffer_append_str(buf, serdec_json_stringify_object(
                data, indent_level + 1, pretty_print));
            break;
        default:
            serdec_buffer_append_str(buf, "\"\"");
            break;
        }
        if (pretty_print)
            serdec_buffer_append_str(buf, data->next ? ",\n" : "\n");
        else
            serdec_buffer_append_str(buf, data->next ? "," : "");
        data = data->next;
    }
    serdec_buffer_putchar(buf, '}');
    char* output = serdec_buffer_take(buf);
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
        (object->json_type != SERDEC_JSON_NULL &&
        object->json_type != SERDEC_JSON_BOOLEAN &&
        object->json_type != SERDEC_JSON_INT &&
        object->json_type != SERDEC_JSON_FLOAT &&
        object->json_type != SERDEC_JSON_STRING))
        return NULL;
    
    serdec_buffer_t* buf = serdec_buffer_new(SERDEC_DEFAULT_BUFFER_CAP);
    if (!buf)
        return NULL;

    if (pretty_print)
        serdec_add_indent(buf, indent_level);

    if (object->key)
        serdec_buffer_printf(buf, "\"%s\":%s", object->key, pretty_print ? " " : "");

    switch (object->json_type) {
    case SERDEC_JSON_NULL:
        serdec_buffer_append_str(buf, "null");
        break;
    case SERDEC_JSON_BOOLEAN:
        serdec_buffer_append_str(buf, object->value.bool_val ? "true" : "false");
        break;
    case SERDEC_JSON_INT:
        serdec_buffer_printf(buf, "%" PRId64, object->value.int_val);
        break;
    case SERDEC_JSON_FLOAT:
        serdec_buffer_printf(buf, "%lf", object->value.float_val);
        break;
    case SERDEC_JSON_STRING:
        serdec_buffer_printf(buf, "\"%s\"", object->value.str_val);
        break;
    default:
        serdec_buffer_append_str(buf, "\"\"");
        break;
    }
    char* output = serdec_buffer_take(buf);
    return output;
}

char* serdec_json_stringify_array(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object || object->json_type != SERDEC_JSON_ARRAY)
        return NULL;
    
    serdec_buffer_t* buf = serdec_buffer_new(SERDEC_DEFAULT_BUFFER_CAP);
    if (!buf)
        return NULL;

    if (pretty_print)
        serdec_add_indent(buf, indent_level);

    if (object->key)
        serdec_buffer_printf(buf, "\"%s\":%s", object->key, pretty_print ? " " : "");
    
    serdec_buffer_append_str(buf, pretty_print ? "[\n" : "[");

    serdec_json_t* element = object->value.children->head;
    while (element) {
        switch (element->json_type) {
        case SERDEC_JSON_ARRAY:
            serdec_buffer_append_str(buf, serdec_json_stringify_array(
                element, indent_level+1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            serdec_buffer_append_str(buf, serdec_json_stringify_object(
                element, indent_level+1, pretty_print));
            break;
        default:
            serdec_buffer_append_str(buf, serdec_json_stringify_primitive(
                element, indent_level+1, pretty_print));
            break;
        }
        if (pretty_print)
            serdec_buffer_append_str(buf, element->next ? ",\n" : "\n");
        else
            serdec_buffer_append_str(buf, element->next ? "," : "");

        element = element->next;
    }
    if (pretty_print)
        serdec_add_indent(buf, indent_level);
    serdec_buffer_putchar(buf, ']');
    char* output = serdec_buffer_take(buf);
    return output;
}

char* serdec_json_stringify_object(serdec_json_t* object, size_t indent_level, bool pretty_print) {
    if (!object || object->json_type != SERDEC_JSON_OBJECT)
        return NULL;
    
    serdec_buffer_t* buf = serdec_buffer_new(SERDEC_DEFAULT_BUFFER_CAP);
    if (!buf)
        return NULL;

    if (pretty_print)
        serdec_add_indent(buf, indent_level);
    
    if (object->key)
        serdec_buffer_printf(buf, "\"%s%s",
            object->key, pretty_print ? "\": {\n" : "\":{");
    else
        serdec_buffer_append_str(buf, pretty_print ? "{\n" : "{");

    serdec_json_t* element = object->value.children->head;
    while (element) {
        switch (element->json_type) {
        case SERDEC_JSON_ARRAY:
            serdec_buffer_append_str(buf, serdec_json_stringify_array(
        element, indent_level+1, pretty_print));
            break;
        case SERDEC_JSON_OBJECT:
            serdec_buffer_append_str(buf, serdec_json_stringify_object(
        element, indent_level+1, pretty_print));
            break;
        default:
            serdec_buffer_append_str(buf, serdec_json_stringify_primitive(
                element, indent_level+1, pretty_print));
            }
        if (pretty_print)
            serdec_buffer_append_str(buf, element->next ? ",\n" :"\n");
        else
            serdec_buffer_append_str(buf, element->next ? "," :"");
        element = element->next;
    }
    if (pretty_print)
        serdec_add_indent(buf, indent_level);

    serdec_buffer_putchar(buf, '}');
    char* output = serdec_buffer_take(buf);
    return output;
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

void serdec_add_indent(serdec_buffer_t* buffer, size_t indent_level) {
    if (!buffer)
        return;
    while (indent_level--)
        serdec_buffer_append_str(buffer, SERDEC_INDENT);
}

serdec_buffer_t* serdec_buffer_new(size_t initial_capacity) {
    size_t capacity = (initial_capacity < SERDEC_DEFAULT_BUFFER_CAP) ?
    SERDEC_DEFAULT_BUFFER_CAP : initial_capacity;

    if (capacity > SIZE_MAX - 1)
        capacity = SIZE_MAX - 2;

    serdec_buffer_t* buf = calloc(1, sizeof(*buf));
    if (!buf)
        return NULL;

    buf->data = malloc(capacity + 1);
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->capacity = capacity + 1;
    buf->length = 0;
    buf->data[0] = '\0';
    return buf;
}

bool serdec_buffer_reserve(serdec_buffer_t* buf, size_t extra_size) {
    if (!buf)
        return false;

    size_t needed_size = buf->length + extra_size + 1;
    if (buf->capacity >= needed_size)
        return true;

    size_t new_capacity = buf->capacity;
    while (new_capacity < needed_size) {
        if (new_capacity > SIZE_MAX / SERDEC_BUFFER_GROWTH_FACTOR)
            return false;
        new_capacity *= SERDEC_BUFFER_GROWTH_FACTOR;
    }

    char* new_data = realloc(buf->data, new_capacity);
    if (!new_data)
        return false;

    buf->data = new_data;
    buf->capacity = new_capacity;
    return true;
}

bool serdec_buffer_append(serdec_buffer_t *dst, const char *src, size_t len) {
    if (!len)
        return true;

    if (!dst || !src)
        return false;
    
    if (!serdec_buffer_reserve(dst, len))
        return false;

    memcpy(dst->data + dst->length, src, len);
    dst->length += len;
    dst->data[dst->length] = '\0';
    return true;
}

bool serdec_buffer_putchar(serdec_buffer_t *buf, char chr) {
    if (!serdec_buffer_reserve(buf, 1))
        return false;

    buf->data[buf->length++] = chr;
    buf->data[buf->length] = '\0';
    return true;
}
bool serdec_buffer_vprintf(serdec_buffer_t* buf, const char* format, va_list args) {
    if (!buf || !format)
        return false;

    size_t available = buf->capacity - buf->length;
    va_list copy;
    va_copy(copy, args);
    int written = vsnprintf(buf->data + buf->length, available, format, copy);
    va_end(copy);
    if (written < 0)
        return false;

    size_t needed = (size_t) written;

    if (needed >= available) {
        if (!serdec_buffer_reserve(buf, needed))
            return false;
        va_list retry;
        va_copy(retry, args);
        written = vsnprintf(buf->data + buf->length,
                            buf->capacity - buf->length,
                            format,
                            retry);
        va_end(retry);
        if (written < 0 || (size_t) written >= buf->capacity - buf->length)
            return false;
        needed = (size_t) written;
    }
    buf->length += needed;
    buf->data[buf->length] = '\0';
    return true;
}

bool serdec_buffer_printf(serdec_buffer_t* buf, const char* format, ...) {
    va_list args;
    va_start(args, format);
    bool success = serdec_buffer_vprintf(buf, format, args);
    va_end(args);
    return success;
}

char* serdec_buffer_take(serdec_buffer_t* buf) {
    if (!buf || !buf->data)
        return NULL;
    char* data = buf->data;
    buf->data = NULL;
    free(buf);
    return data;
}

size_t serdec_buffer_length(serdec_buffer_t *buf) {
    return buf->length;
}

void serdec_buffer_clear(serdec_buffer_t *buf) {
    if (!buf || !buf->data)
        return;

    buf->length = 0;
    buf->data[0] = '\0';
}

void serdec_buffer_free(serdec_buffer_t *buf) {
    if (!buf)
        return;

    free(buf->data);
    free(buf);
}
