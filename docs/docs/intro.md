---
slug: /
sidebar_position: 1
---

# Introduction
Serdec is a C serialization and deserialization library. 

It provides simple APIs for constructing JSON trees, serializing to strings, and parsing JSON strings back into structured C objects.

In the future, it will support multiple data serialzation format (e.g. YAML, TOML, XML, CSV).

## Core Functions

### Creation Functions

| Function | Description | Return Value |
|:---|:---|:---|
| `serdec_json_new_object(void)` | Create a new empty JSON object `{}`. | `serdec_json_t*` |
| `serdec_json_new_array(void)` | Create a new empty JSON array `[]`. | `serdec_json_t*` |
| `serdec_json_new_null(void)` | Create a JSON `null` value node. | `serdec_json_t*` |
| `serdec_json_new_bool(bool value)` | Create a JSON `true`/`false` value node. | `serdec_json_t*` |
| `serdec_json_new_int(int64_t value)` | Create a JSON integer value node. | `serdec_json_t*` |
| `serdec_json_new_float(double value)` | Create a JSON floating-point value node. | `serdec_json_t*` |
| `serdec_json_new_string(const char* str)` | Create a JSON string value node (copies the string). | `serdec_json_t*` |

---

### Adding Fields to Objects

| Function | Description | Return Value |
|:---|:---|:---|
| `serdec_json_add_null(serdec_json_t* object, const char* key, void* null)` | Add a null field into an object. | `bool` |
| `serdec_json_add_bool(serdec_json_t* object, const char* key, bool value)` | Add a boolean field into an object. | `bool` |
| `serdec_json_add_int(serdec_json_t* object, const char* key, int64_t value)` | Add an integer field into an object. | `bool` |
| `serdec_json_add_float(serdec_json_t* object, const char* key, double value)` | Add a float field into an object. | `bool` |
| `serdec_json_add_string(serdec_json_t* object, const char* key, const char* value)` | Add a string field into an object. | `bool` |
| `serdec_json_add_array(serdec_json_t* object, const char* key, serdec_json_t* array)` | Add an array as a field into an object. | `bool` |
| `serdec_json_add_object(serdec_json_t* object, const char* key, serdec_json_t* value)` | Add a nested object field into an object. | `bool` |

---

### Manipulating Arrays

| Function | Description | Return Value |
|:---|:---|:---|
| `serdec_json_array_add(serdec_json_t* array, serdec_json_t* value)` | Add a value into an array. | `bool` |

---

### Serialization and Deserialization

| Function | Description | Return Value |
|:---|:---|:---|
| `serdec_json_stringify(const serdec_json_t* object)` | Serialize a JSON tree into a compact string. | `char*` (must `free()`) |
| `serdec_json_pretty_stringify(const serdec_json_t* object)` | Serialize a JSON tree into a human-readable formatted string. | `char*` (must `free()`) |
| `serdec_json_parse(const char* json_string)` | Parse a JSON string into an in-memory tree. | `serdec_json_t*` |

### Memory Management

| Function | Description | Return Value |
|:---|:---|:---|
| `serdec_json_free(serdec_json_t* node)` | Recursively free an entire JSON structure. | `void` |

## 🌟 Optional/Advanced Functions

| Function | Description | Return Value |
|:---|:---|:---|
| `serdec_json_typeof(const serdec_json_t* node)` | Get the type of a JSON node. | `serdec_json_type` |
| `serdec_json_object_get(const serdec_json_t* object, const char* key)` | Lookup a field by key from an object. | `serdec_json_t*` |
| `serdec_json_array_get(const serdec_json_array_t* array, size_t index)` | Retrieve an element by index from an array. | `serdec_json_t*` |
| `serdec_json_clone(const serdec_json_t* node)` | Deep copy a full JSON node tree. | `serdec_json_t*` |
| `serdec_json_merge(serdec_json_t* target, const serdec_json_t* source)` | Merge fields from one object into another. | `bool` |
| `serdec_json_get_last_error(void)` | Get a description of the last error that occurred. | `const char*` |
| `serdec_json_clear_error(void)` | Clear the last error. | `void` |

## 📋 JSON Type Enumeration

```c
typedef enum {
    SERDEC_JSON_NULL,
    SERDEC_JSON_BOOLEAN,
    SERDEC_JSON_INT,
    SERDEC_JSON_FLOAT,
    SERDEC_JSON_STRING,
    SERDEC_JSON_ARRAY,
    SERDEC_JSON_OBJECT
} serdec_json_type;
```
- Used by `serdec_json_typeof()` to determine node types at runtime.

## 📌 Notes

- Always free memory returned by `serdec_json_stringify` and `serdec_json_pretty_stringify` using `free()`.
- `serdec_json_free()` automatically frees all child nodes recursively.
- If you use C11, you can use `_Generic` macro-based `serdec_json_add(object, key, value)` for automatic type dispatching.
- If using C99, call `serdec_json_add_int`, `serdec_json_add_string`, etc., manually.

# 🌟 Example Usage

```c
serdec_json_t* root = serdec_json_new_object();
serdec_json_add_string(root, "name", "Leorium");
serdec_json_add_int(root, "age", 24);

serdec_json_t* hobbies = serdec_json_new_array();
serdec_json_array_add(hobbies, serdec_json_new_string("C Programming"));
serdec_json_array_add(hobbies, serdec_json_new_string("Motorcycling"));

serdec_json_add_object(root, "hobbies", hobbies);

char* output = serdec_json_stringify(root);
printf("%s\n", output);

serdec_json_free(root);
free(output);
```