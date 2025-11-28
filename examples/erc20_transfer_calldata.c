#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

/*
 * Simple CLI tool that builds ERC-20 transfer calldata.
 *
 * Usage:
 *   ./erc20_transfer_calldata <to_address_hex> <amount_uint64>
 *
 * Examples:
 *   ./erc20_transfer_calldata 0x00112233445566778899aAbBcCdDeEfF00112233 1000
 *
 * It prints the calldata as a hex string prefixed with 0x.
 */

/* Strip optional "0x" or "0X" prefix from a hex string. */
static const char *strip_0x(const char *s) {
    if (s == NULL) {
        return NULL;
    }
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return s + 2;
    }
    return s;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr,
                "Usage:\n"
                "  %s <to_address_hex> <amount_uint64>\n\n"
                "Example:\n"
                "  %s 0x00112233445566778899aAbBcCdDeEfF00112233 1000\n",
                argv[0], argv[0]);
        return 1;
    }

    const char *addr_str = strip_0x(argv[1]);
    const char *amount_str = argv[2];

    /* Basic validation of address hex length (40 hex chars = 20 bytes). */
    size_t addr_hex_len = strlen(addr_str);
    if (addr_hex_len != 40) {
        fprintf(stderr,
                "Error: expected a 20-byte (40 hex chars) address, got length %zu.\n",
                addr_hex_len);
        return 1;
    }

    /* Decode address from hex into 20 bytes. */
    uint8_t addr_bytes[20];
    int decoded = web3c_hex_decode(addr_str, addr_bytes, sizeof(addr_bytes));
    if (decoded != 20) {
        fprintf(stderr,
                "Error: failed to decode address hex, decoded bytes = %d.\n",
                decoded);
        return 1;
    }

    /* Parse amount as uint64_t. */
    char *endptr = NULL;
    unsigned long long amount_ull = strtoull(amount_str, &endptr, 10);
    if (endptr == amount_str || *endptr != '\0') {
        fprintf(stderr, "Error: invalid amount '%s'. Expected unsigned integer.\n", amount_str);
        return 1;
    }

    uint64_t amount = (uint64_t)amount_ull;

    /* Compute function selector for transfer(address,uint256). */
    unsigned char selector[4];
    if (web3c_abi_function_selector("transfer(address,uint256)", selector) != 0) {
        fprintf(stderr, "Error: web3c_abi_function_selector failed.\n");
        return 1;
    }

    /* Encode address and amount as ABI words. */
    unsigned char addr_word[WEB3C_ABI_WORD_SIZE];
    unsigned char amount_word[WEB3C_ABI_WORD_SIZE];

    if (web3c_abi_encode_address(addr_bytes, addr_word) != 0) {
        fprintf(stderr, "Error: web3c_abi_encode_address failed.\n");
        return 1;
    }

    if (web3c_abi_encode_uint256(amount, amount_word) != 0) {
        fprintf(stderr, "Error: web3c_abi_encode_uint256 failed.\n");
        return 1;
    }

    /* Build full calldata: 4 bytes selector + 32 bytes address + 32 bytes amount. */
    unsigned char calldata[4 + WEB3C_ABI_WORD_SIZE + WEB3C_ABI_WORD_SIZE];
    size_t calldata_len = sizeof(calldata);

    memcpy(calldata, selector, 4);
    memcpy(calldata + 4, addr_word, WEB3C_ABI_WORD_SIZE);
    memcpy(calldata + 4 + WEB3C_ABI_WORD_SIZE, amount_word, WEB3C_ABI_WORD_SIZE);

    /* Hex-encode calldata. */
    char calldata_hex[(4 + WEB3C_ABI_WORD_SIZE + WEB3C_ABI_WORD_SIZE) * 2 + 1];
    if (web3c_hex_encode(calldata, calldata_len, calldata_hex, sizeof(calldata_hex)) != 0) {
        fprintf(stderr, "Error: web3c_hex_encode failed.\n");
        return 1;
    }

    printf("ERC-20 transfer calldata:\n");
    printf("  to:     0x%s\n", addr_str);
    printf("  amount: %llu\n", amount_ull);
    printf("  data:   0x%s\n", calldata_hex);

    return 0;
}