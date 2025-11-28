#include "web3c/abi.h"

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