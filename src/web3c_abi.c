#include "web3c/abi.h"
#include "web3c/keccak.h"

#include <string.h>

int web3c_abi_encode_uint256(uint64_t value, unsigned char *out) {
    if (out == NULL) {
        return -1;
    }

    /* Zero the entire 32-byte word first. */
    memset(out, 0, WEB3C_ABI_WORD_SIZE);

    /*
     * Write the uint64_t value in big-endian form into
     * the last 8 bytes of the word.
     */
    for (int i = 0; i < 8; ++i) {
        out[WEB3C_ABI_WORD_SIZE - 1 - i] = (unsigned char)((value >> (8 * i)) & 0xFF);
    }

    return 0;
}

int web3c_abi_encode_address(const unsigned char *address, unsigned char *out) {
    if (address == NULL || out == NULL) {
        return -1;
    }

    /* Zero the entire 32-byte word. */
    memset(out, 0, WEB3C_ABI_WORD_SIZE);

    /*
     * Copy the 20-byte address into the last 20 bytes of the word.
     */
    memcpy(out + (WEB3C_ABI_WORD_SIZE - 20), address, 20);

    return 0;
}

int web3c_abi_encode_bool(int value, unsigned char *out) {
    if (out == NULL) {
        return -1;
    }

    /* Zero the entire 32-byte word. */
    memset(out, 0, WEB3C_ABI_WORD_SIZE);

    /*
     * According to Solidity ABI:
     * - false => 0
     * - true  => 1
     *
     * We interpret any non-zero value as true.
     */
    out[WEB3C_ABI_WORD_SIZE - 1] = (value != 0) ? 0x01 : 0x00;

    return 0;
}

int web3c_abi_encode_bytes32(const unsigned char *value, unsigned char *out) {
    if (value == NULL || out == NULL) {
        return -1;
    }

    /*
     * bytes32 is already a fixed-size 32-byte value,
     * so we simply copy the 32 bytes as-is.
     */
    memcpy(out, value, WEB3C_ABI_WORD_SIZE);
    return 0;
}

int web3c_abi_function_selector(const char *signature, unsigned char out[4]) {
    if (signature == NULL || out == NULL) {
        return -1;
    }

    uint8_t hash[32];

    /* Compute keccak256(signature) as used by Ethereum for function selectors. */
    if (web3c_keccak256((const uint8_t *)signature, strlen(signature), hash) != 0) {
        return -1;
    }

    /* First 4 bytes are the function selector. */
    memcpy(out, hash, 4);

    return 0;
}