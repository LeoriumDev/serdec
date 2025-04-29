# Compiler
CC = clang
CFLAGS = -Wall -g -O0 -fno-omit-frame-pointer -Wextra -Wpedantic -Wshadow -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wundef -Wredundant-decls -std=c2y -Iinclude
# Directories
BUILD_DIR = build
SRC = src/serdec_json.c
OBJ = $(BUILD_DIR)/serdec_json.o
TARGET = $(BUILD_DIR)/libserdec_json.a

# Test
TEST_SRC = tests/test_basic.c
TEST_BIN = $(BUILD_DIR)/test_basic

# Examples
EXAMPLES_SRC = $(wildcard examples/*.c)
EXAMPLES_BIN = $(patsubst examples/%.c, $(BUILD_DIR)/%, $(EXAMPLES_SRC))

.PHONY: all clean test examples run-tests

# Default target
all: $(TARGET)

# Build static library
$(TARGET): $(OBJ)
	ar rcs $@ $^

# Object build rule
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build dir if not exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build and run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC) $(SRC) -o $@

# Build all examples
examples: $(EXAMPLES_BIN)

# Run all examples (with timeout) and save pure output to build/test.json
run-examples: examples
	@mkdir -p build
	@> build/test.json # clear previous output
	@for bin in $(EXAMPLES_BIN); do \
		timeout 10s $$bin >> build/test.json 2>&1 || echo "$$bin timed out" >> build/test.json; \
	done

# Rule for each example
$(BUILD_DIR)/%: examples/%.c $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)