# serdec
A lightweight serialization and deserialization C library for JSON.

## Building

Requires CMake 4.2.3+ and a C23 compiler.

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Run tests
ctest --test-dir build
```

### Build Options

| Option | Description |
|--------|-------------|
| `-DSERDEC_ENABLE_SANITIZERS=ON` | Enable ASan + UBSan |
| `-DSERDEC_ENABLE_ASAN=ON` | Enable AddressSanitizer only |
| `-DSERDEC_ENABLE_UBSAN=ON` | Enable UndefinedBehaviorSanitizer only |

Development build with sanitizers:

```bash
cmake -B build -DSERDEC_ENABLE_SANITIZERS=ON
cmake --build build
```
