CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -pedantic -Iinclude
AR      := ar
ARFLAGS := rcs

SRC_DIR := src
TEST_DIR := tests
EXAMPLES_DIR := examples
BUILD_DIR := build

LIB_NAME := libweb3c.a

SRC := $(SRC_DIR)/web3c_abi.c \
       $(SRC_DIR)/web3c_hex.c

OBJS := $(SRC:%.c=$(BUILD_DIR)/%.o)

TEST_BIN := $(BUILD_DIR)/test_abi
EXAMPLE_BIN := $(BUILD_DIR)/simple_encode

all: $(BUILD_DIR)/$(LIB_NAME) $(TEST_BIN) $(EXAMPLE_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/$(SRC_DIR) \
	         $(BUILD_DIR)/$(TEST_DIR) \
	         $(BUILD_DIR)/$(EXAMPLES_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(LIB_NAME): $(OBJS) | $(BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(TEST_BIN): $(TEST_DIR)/test_abi.c $(BUILD_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) -L$(BUILD_DIR) -lweb3c $< -o $@

$(EXAMPLE_BIN): $(EXAMPLES_DIR)/simple_encode.c $(BUILD_DIR)/$(LIB_NAME)
	$(CC) $(CFLAGS) -L$(BUILD_DIR) -lweb3c $< -o $@

test: $(TEST_BIN)
	$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
