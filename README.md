# Serdec
A lightweight C serialization and deserialization library for working with JSON (and future formats like YAML, TOML, XML, CSV).
---

## ✨ Features

- Simple and clean C API
- Create JSON objects, arrays, and values
- Serialize JSON to string
- Parse JSON strings back into structured C memory (planned)
- Lightweight, portable, C23 compatible
- Designed for extensibility (YAML, XML support in the future)

---

## 📦 Installation

Clone the repository:

```bash
git clone https://github.com/LeoriumDev/serdec.git
```

Add the `src/` files to your project:

```plaintext
serdec.c
serdec.h
```

Compile together with your code:

```bash
gcc your_code.c serdec.c -o your_program
```

---

## 📝 Example Usage

```c
#include <stdio.h>
#include "serdec.h"

int main(void) {
    serdec_json_t* root = serdec_json_new_object();
    serdec_json_add_string(root, "name", "Leorium");
    serdec_json_add_int(root, "age", 20);

    serdec_json_t* hobbies = serdec_json_new_array();
    serdec_json_array_add(hobbies, serdec_json_new_string("C Programming"));
    serdec_json_array_add(hobbies, serdec_json_new_string("Motorcycling"));

    serdec_json_add_object(root, "hobbies", hobbies);

    char* output = serdec_json_stringify(root);
    printf("%s\n", output);

    serdec_json_free(root);
    free(output);

    return EXIT_SUCCESS;
}
```

---

## 🛠️ API Overview

### Creation Functions

- `serdec_json_new_object(void)`
- `serdec_json_new_array(void)`
- `serdec_json_new_null(void)`
- `serdec_json_new_bool(bool value)`
- `serdec_json_new_int(int64_t value)`
- `serdec_json_new_float(double value)`
- `serdec_json_new_string(const char* string)`

### Adding Fields

- `serdec_json_add_null`
- `serdec_json_add_bool`
- `serdec_json_add_int`
- `serdec_json_add_float`
- `serdec_json_add_string`
- `serdec_json_add_array`
- `serdec_json_add_object`

### Serialization

- `char* serdec_json_stringify(const serdec_json_t* object)`
- *(Planned)* `char* serdec_json_pretty_stringify(const serdec_json_t* object)`

### Memory Management

- `void serdec_json_free(serdec_json_t* node)`

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

---

## 🚀 Roadmap

- [x] JSON serialization
- [x] JSON object and array creation
- [x] Memory-safe node creation
- [ ] JSON deserialization (parsing)
- [ ] Pretty-printing JSON
- [ ] YAML serialization/deserialization
- [ ] XML serialization/deserialization

---

## 🤝 Contributing

Pull requests are welcome!  
For major changes, please open an issue first to discuss what you would like to change.

---

## ❤️ Acknowledgements

- Inspired by lightweight C libraries like cJSON and jansson.
- Built from scratch for full control over serialization and future extensibility.

