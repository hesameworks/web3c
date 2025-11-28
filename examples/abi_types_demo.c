#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

/*
 * Demonstration of encoding several ABI types:
 *   - uint256 (from uint64_t)
 *   - bool
 *   - bytes32
 */

static void print_hex(const unsigned char *data, size_t len) {
    char hex_buf[WEB3C_ABI_WORD_SIZE * 2 + 1];

    if (web3c_hex_encode(data, len, hex_buf, sizeof(hex_buf)) != 0) {
        fprintf(stderr, "hex encode failed\n");
        return;
    }

    printf("0x%s\n", hex_buf);
}

int main(void) {
    unsigned char word[WEB3C_ABI_WORD_SIZE];

    /* Encode uint256(42) */
    if (web3c_abi_encode_uint256(42, word) != 0) {
        fprintf(stderr, "web3c_abi_encode_uint256 failed\n");
        return 1;
    }
    printf("ABI uint256(42):\n  ");
    print_hex(word, WEB3C_ABI_WORD_SIZE);

    /* Encode bool(true) */
    if (web3c_abi_encode_bool(1, word) != 0) {
        fprintf(stderr, "web3c_abi_encode_bool failed\n");
        return 1;
    }
    printf("ABI bool(true):\n  ");
    print_hex(word, WEB3C_ABI_WORD_SIZE);

    /* Encode bytes32("web3c") padded with zeros */
    unsigned char bytes32_val[WEB3C_ABI_WORD_SIZE];
    memset(bytes32_val, 0, sizeof(bytes32_val));
    memcpy(bytes32_val, "web3c", 5);

    if (web3c_abi_encode_bytes32(bytes32_val, word) != 0) {
        fprintf(stderr, "web3c_abi_encode_bytes32 failed\n");
        return 1;
    }
    printf("ABI bytes32(\"web3c\"):\n  ");
    print_hex(word, WEB3C_ABI_WORD_SIZE);

    return 0;
}