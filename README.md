# serdec

A **Ser**ialization/**De**serialization library for JSON, in **C**.

> **Status:** Work in progress

## Building

Requires CMake 4.2.3+ and a C23 compiler.

```bash
make build
```

### Build Options

| Option | Description |
|--------|-------------|
| `-DSERDEC_ENABLE_SANITIZERS=ON` | Enable ASan + UBSan |
| `-DSERDEC_ENABLE_ASAN=ON` | Enable AddressSanitizer only |
| `-DSERDEC_ENABLE_UBSAN=ON` | Enable UndefinedBehaviorSanitizer only |

## Testing

```bash
# Build and run all tests
make test

# Build with sanitizers and run tests
make sanitize

# Run specific suite
./build/tests/serdec_tests buffer
./build/tests/serdec_tests arena
```

## Development

```bash
make build     # Configure + build
make test      # Build + run tests
make sanitize  # Build with ASan/UBSan + run tests
make clean     # Remove build directory
make rebuild   # Clean + build
```
