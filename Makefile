CC      = gcc
AR      = ar
ARFLAGS = rcs

CFLAGS   = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Iinclude

# Core library sources
SRC = \
    src/web3c_abi.c \
    src/web3c_hex.c \
    src/web3c_keccak.c

OBJ = $(SRC:.c=.o)

LIB = libweb3c.a

# Test binaries
TEST_BINS = \
    tests/test_abi \
    tests/test_keccak

.PHONY: all clean tests test

# Default target: build the static library
all: $(LIB)

# Build static library
$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

# Generic rule for building objects
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build test binaries (each test links against libweb3c)
tests/test_abi: tests/test_abi.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

tests/test_keccak: tests/test_keccak.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

# Build all tests (but do not run them)
tests: $(TEST_BINS)

# Build and run test suite
test: $(LIB) tests
	@echo "Running tests..."
	@./tests/test_abi
	@./tests/test_keccak
	@echo "All tests passed."

clean:
	rm -f $(OBJ) $(LIB) $(TEST_BINS)