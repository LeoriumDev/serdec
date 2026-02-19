# serdec

A **Ser**ialization/**De**serialization library for JSON, in **C**.

## Overview

serdec is a JSON parsing and serialization library for C, designed for correctness and throughput.

It has two first-class consumption modes. The event iterator is pull-based with zero DOM
allocation, suitable for streaming and selective extraction. No per-event heap allocations occur;
escapes allocate only when materialized. The DOM builder produces an arena-backed value tree built
on top of the same event core.

Lifetime model: the arena owns nodes. String bytes are borrowed from the input buffer by default,
so the input must outlive parsed values. Copying strings into the arena is opt-in. Escapes and
surrogate pairs are validated; unescaping is materialized lazily when requested.

Target behavior: strict RFC 8259 only. Non-conforming JSON (comments, trailing commas, NaN/Inf,
leading zeros) will be rejected. Full UTF-8 validation on all input is planned.

```c
SerdecArena *arena = serdec_arena_create(NULL);
SerdecValue *root  = NULL;
SerdecError  err;

if (serdec_parse(arena, json, json_len, &root, &err) != SERDEC_OK) {
    fprintf(stderr, "parse error at offset %zu: %s\n", err.offset, err.message);
    serdec_arena_destroy(arena);
    return 1;
}

// happy path (error handling omitted for brevity)
const SerdecValue *v    = serdec_get(root, "name"); // planned: status+out-param in 0.4.0
SerdecString       name = serdec_as_string(v);
printf("%.*s\n", (int)name.len, name.ptr);

serdec_arena_destroy(arena); // frees root + all nodes in one shot
```

> **Note:** The snippet uses the planned `0.4.0` API and may change.

> **Status:** Work in progress — not yet ready for production use. Some guarantees described above are design targets, not yet fully implemented. See the [Roadmap](#roadmap).

## Target Architecture

```
  Public
┌─────────────────────────────────────────┐
│   High-Level API                        │
│   (serdec_parse, serdec_write, config)  │
├──────────────────────┬──────────────────┤
│   Event Iterator     │   DOM + Query    │
│   (serdec_event_next,│   (serdec_get,   │
│    pull-based)       │    serdec_as_*)  │
├──────────────────────┴──────────────────┤
│   Writer                                │
│   (DOM -> JSON, compact + pretty)       │
└─────────────────────────────────────────┘
  Internal
┌─────────────────────────────────────────┐
│   UTF-8 / string unescape / numbers     │
├─────────────────────────────────────────┤
│   Lexer + parser state machine          │
│   (feeds the event iterator)            │
├─────────────────────────────────────────┤
│   Foundation                            │
│   (arena, buffer, error)                │
└─────────────────────────────────────────┘
```

> The DOM builder consumes the event iterator. The writer consumes the DOM. Both consumption
> modes are first-class public APIs.

## Versioning

serdec follows [Semantic Versioning](https://semver.org). The public API is not stable until
`1.0.0`. Before that, minor version bumps (`0.x.0`) may include breaking changes.

## Roadmap

**Target standard:** RFC 8259 (strict) — no comments, no trailing commas, no NaN/Inf, no leading
zeros.

> This roadmap is aspirational; scope may shift as test coverage and fuzzing results come in.

---

### `0.1.0` — Foundation, UTF-8 & string
- [x] Arena allocator, buffer, error model
- [x] Lexer core
- [ ] UTF-8 validation (reject invalid, overlong, surrogates, out-of-range)
- [ ] Unicode escape decoding for strings (`\uXXXX` + surrogate pairs)
- [ ] JSON string unescape
- [ ] CI passing

### `0.2.0` — Lexer hardening
- [ ] JSONTestSuite (strict RFC 8259 profile): target all must-pass green; document known non-goals
- [ ] Lexer fuzz target + initial corpus

### `0.3.0` — Event iterator
- [ ] Pull-based event API (`serdec_event_next`)
- [ ] Zero allocations per event in the hot path (aside from parser init; escapes allocate only
      when materialized)
- [ ] Strings as borrowed slices into input by default
- [ ] Parse errors include byte offset
- [ ] Depth + size limits (hard error, not truncation)
- [ ] Scalar baseline benchmark published

### `0.4.0` — SIMD stage 1
- [ ] Vectorized whitespace skipping + structural character scan (SSE2/AVX2/NEON)
- [ ] Fast string scanning (find closing quote, handle escapes)
- [ ] Scalar fallback for all unsupported targets
- [ ] Benchmark vs cJSON/jansson published (big jump expected here)

### `0.5.0` — DOM builder + Query API
- [ ] `SerdecValue` tree (object / array / string / number / bool / null) built on event iterator
- [ ] Numbers as tagged union: `int64` + `uint64` + `double` + optional raw slice
- [ ] Copy-into-arena opt-in; borrowed slices default
- [ ] DOM invariants decided + documented: duplicate keys, ordering, limit behavior
- [ ] `serdec_get`, `serdec_index`
- [ ] `serdec_as_string`, `serdec_as_number`, `serdec_as_bool`
- [ ] Explicit error signaling: no silent NULLs, no implicit type coercions
- [ ] Parser fuzz target

### `0.6.0` — Serializer
- [ ] `serdec_write`: DOM -> JSON (compact + pretty-print)
- [ ] Lone surrogates rejected; valid surrogate pairs accepted; output policy documented
- [ ] Fast float-to-string (Ryu or equivalent)
- [ ] Lax parsing mode (opt-in, non-RFC: allow comments, trailing commas)

### `0.7.0` — SIMD stage 2 + fast numbers
- [ ] SIMD-accelerated UTF-8 validation (hybrid with scalar fallback)
- [ ] Fast integer parsing
- [ ] Fast float parsing (Ryu or equivalent)

### `0.8.0` — Bench harness + portability
- [ ] Reproducible benchmark runner + dataset pack
- [ ] Per-arch SIMD dispatch policy documented
- [ ] Portability verified: scalar fallback tested on targets without SIMD

### `0.9.0` — Hardening & API freeze
- [ ] ASan + UBSan + TSan clean
- [ ] Thread safety documented and verified
- [ ] Full parser + lexer fuzz corpus
- [ ] Benchmarks published (event iterator mode unless stated otherwise): target within 20% of
      yyjson scalar, 80% with SIMD
- [ ] API reference complete: ownership + error rules documented for every public symbol
- [ ] Aim: no planned breaking changes

### `1.0.0` — Stable release
- [ ] ABI contract written: opaque public structs, binary compatible within `1.x` (stable exported
      symbols + opaque handles)
- [ ] Config struct carries version + reserved fields
- [ ] CI matrix: Linux / macOS / Windows
- [ ] Full documentation + changelog

### Post-1.0 (v2.0)
- JSON Pointer (RFC 6901)
- JSON Patch (RFC 6902)
- Schema validation
- Advanced SIMD optimizations

---

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

## Contributing

Commits follow [Conventional Commits 1.0.0](https://www.conventionalcommits.org/en/v1.0.0/).

## References
- [RFC 8259 - The JavaScript Object Notation (JSON) Data Interchange Format](https://www.rfc-editor.org/rfc/rfc8259)
- [RFC 3629 - UTF-8, a transformation format of ISO 10646](https://www.rfc-editor.org/rfc/rfc3629)
- [Conventional Commits 1.0.0](https://www.conventionalcommits.org/en/v1.0.0/)
- [Semantic Versioning 2.0.0](https://semver.org)
