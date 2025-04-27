# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c23 -Iinclude

# Source and output
SRC = src/serdec_json.c
OBJ = $(SRC:.c=.o)
TARGET = libserdec_json.a

# Test
TEST_SRC = tests/test_basic.c
TEST_BIN = tests/test_basic

.PHONY: all clean test

EXAMPLES = examples/example_basic examples/example_array examples/example_nested

examples: $(EXAMPLES)

examples/example_basic: examples/example_basic.c src/serdec_json.c
	$(CC) $(CFLAGS) $^ -o $@

examples/example_array: examples/example_array.c src/serdec_json.c
	$(CC) $(CFLAGS) $^ -o $@

examples/example_nested: examples/example_nested.c src/serdec_json.c
	$(CC) $(CFLAGS) $^ -o $@

# Default target
all: $(TARGET)

# Build static library
$(TARGET): $(OBJ)
	ar rcs $@ $^

# Build and run tests
test: $(TARGET) $(TEST_SRC)
	$(CC) $(CFLAGS) $(TEST_SRC) src/serdec_json.c -o $(TEST_BIN)
	./$(TEST_BIN)

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET) $(TEST_BIN)