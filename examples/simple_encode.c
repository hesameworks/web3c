#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

int main(void) {
    unsigned char abi_word[WEB3C_ABI_WORD_SIZE];
    char hex[WEB3C_ABI_WORD_SIZE * 2 + 1];

    uint64_t value = 42;

    if (web3c_abi_encode_uint256(value, abi_word) != 0) {
        fprintf(stderr, "Failed to encode uint256.\n");
        return 1;
    }

    if (web3c_hex_encode(abi_word, WEB3C_ABI_WORD_SIZE, hex, sizeof(hex)) != 0) {
        fprintf(stderr, "Failed to encode hex.\n");
        return 1;
    }

    printf("Value: %llu\n", (unsigned long long)value);
    printf("ABI word (hex): %s\n", hex);

    return 0;
}
