CC      = gcc
AR      = ar
ARFLAGS = rcs

CFLAGS   = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS = -Iinclude

# Core library sources
SRC = \
    src/web3c_abi.c \
    src/web3c_hex.c \
    src/web3c_keccak.c \
	src/web3c_tx.c

OBJ = $(SRC:.c=.o)

LIB = libweb3c.a

# Test sources and binaries
TEST_SRCS = \
    tests/test_abi.c \
    tests/test_keccak.c \
    tests/test_selector.c \
	tests/test_tx.c

TEST_BINS = $(TEST_SRCS:.c=)

# Example sources and binaries
EXAMPLE_SRCS = \
    examples/simple_encode.c \
    examples/erc20_transfer_calldata.c \
    examples/abi_types_demo.c \
	examples/setdata_bytes_calldata.c \
	examples/tx_legacy_demo.c

EXAMPLE_BINS = $(EXAMPLE_SRCS:.c=)

.PHONY: all tests test examples clean

# Default: only build the static library
all: $(LIB) tests examples

# Build static library
$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

# Generic rule for building objects
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build test binaries (each test links against libweb3c)
tests/%: tests/%.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

# Build example binaries (each example links against libweb3c)
examples/%: examples/%.c $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

# Build all examples
examples: $(EXAMPLE_BINS)

# Build all tests (but do not run them)
tests: $(TEST_BINS)

# Build and run test suite
test: $(LIB) tests
	@echo "Running tests..."
	@./tests/test_abi
	@./tests/test_keccak
	@./tests/test_selector
	@./tests/test_tx
	@echo "All tests passed."

clean:
	rm -f $(OBJ) $(LIB) $(TEST_BINS) $(EXAMPLE_BINS)