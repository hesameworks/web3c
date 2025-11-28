#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

/* Small helper: convert 4 bytes to hex for debug output. */
static void bytes4_to_hex(const uint8_t in[4], char out[9]) {
    static const char *hex = "0123456789abcdef";

    for (int i = 0; i < 4; ++i) {
        uint8_t b = in[i];
        out[2 * i]     = hex[b >> 4];
        out[2 * i + 1] = hex[b & 0x0F];
    }
    out[8] = '\0';
}

static int test_transfer_selector(void) {
    /* Solidity: keccak256("transfer(address,uint256)")[:4] = a9059cbb */
    const char *sig = "transfer(address,uint256)";
    const uint8_t expected[4] = { 0xa9, 0x05, 0x9c, 0xbb };

    uint8_t selector[4];
    char    hex[9];

    if (web3c_abi_function_selector(sig, selector) != 0) {
        fprintf(stderr, "web3c_abi_function_selector() returned error.\n");
        return 1;
    }

    if (memcmp(selector, expected, 4) != 0) {
        bytes4_to_hex(selector, hex);
        fprintf(stderr,
                "Selector mismatch for %s:\n"
                "  got:      %s\n"
                "  expected: a9059cbb\n",
                sig, hex);
        return 1;
    }

    return 0;
}

static int test_approve_selector(void) {
    /* approve(address,uint256) -> 095ea7b3 */
    const char *sig = "approve(address,uint256)";
    const uint8_t expected[4] = { 0x09, 0x5e, 0xa7, 0xb3 };

    uint8_t selector[4];
    char    hex[9];

    if (web3c_abi_function_selector(sig, selector) != 0) {
        fprintf(stderr, "web3c_abi_function_selector() returned error.\n");
        return 1;
    }

    if (memcmp(selector, expected, 4) != 0) {
        bytes4_to_hex(selector, hex);
        fprintf(stderr,
                "Selector mismatch for %s:\n"
                "  got:      %s\n"
                "  expected: 095ea7b3\n",
                sig, hex);
        return 1;
    }

    return 0;
}

int main(void) {
    printf("Running Web3C function selector tests...\n");

    if (test_transfer_selector() != 0) {
        return 1;
    }
    if (test_approve_selector() != 0) {
        return 1;
    }

    printf("All function selector tests passed.\n");
    return 0;
}