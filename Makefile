# Compiler and flags
CC             := clang
CFLAGS         := -Wall -Wextra -Wpedantic -Wshadow -Wcast-qual \
                  -Wstrict-prototypes -Wmissing-prototypes -Wundef \
                  -Wredundant-decls -std=c23 -g -O0 -fno-omit-frame-pointer \
                  -Iinclude -DSERDEC_LOG_LEVEL=0
CFLAGS_RELEASE := $(filter-out -g -O0 -DSERDEC_LOG_LEVEL=0,$(CFLAGS)) -O2

# Directories and targets
BUILD_DIR  := build
SRC        := src/serdec_json.c src/serdec_arena.c src/serdec_vector.c src/serdec_string.c
OBJ        := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))
TARGET     := $(BUILD_DIR)/libserdec_json.a

# Tests
TEST_SRC := tests/test_basic.c
TEST_BIN := $(BUILD_DIR)/test_basic

# Examples
EXAMPLES_SRC := $(wildcard examples/*.c)
EXAMPLES_BIN := $(patsubst examples/%.c,$(BUILD_DIR)/%,$(EXAMPLES_SRC))

.PHONY: all clean test examples run-examples lint release

# Default: build library
all: $(TARGET)

# Static library
$(TARGET): $(OBJ)
	ar rcs $@ $^

# Compile source files
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

# Ensure build directory exists
$(BUILD_DIR):
	@mkdir -p $@

# Test build and run
test: $(TEST_BIN)
	@./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(SRC) | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $^ -o $@

# Examples build
examples: $(EXAMPLES_BIN)

$(EXAMPLES_BIN): $(BUILD_DIR)/%: examples/%.c $(SRC) | $(BUILD_DIR)
	@$(CC) $(CFLAGS) $^ -o $@

# Run all examples, capture JSON output and logs
run-examples: examples
	@mkdir -p build
	@for bin in $(EXAMPLES_BIN); do \
		base=$$(basename $$bin); \
		json_out="build/$$base.json"; \
		log_out="build/$$base.json.log"; \
		echo "🚀 Running $$bin > $$json_out"; \
		time $$bin > "$$json_out" 2> "$$log_out"; \
		[ -s "$$log_out" ] || rm -f "$$log_out"; \
	done

# Static analysis
lint:
	@clang-tidy src/*.c examples/*.c tests/*.c -- -Iinclude

# Clean build artifacts
clean:
	@rm -rf $(BUILD_DIR)

release: CFLAGS := $(CFLAGS_RELEASE)
release: all
	@strip $(TARGET)