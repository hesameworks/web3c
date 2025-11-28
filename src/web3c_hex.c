#include "web3c/hex.h"

static int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

int web3c_hex_encode(const uint8_t *in, size_t in_len, char *out, size_t out_size) {
    static const char *hex_chars = "0123456789abcdef";

    if (in == NULL || out == NULL) {
        return -1;
    }

    if (out_size < (in_len * 2 + 1)) {
        return -1;
    }

    for (size_t i = 0; i < in_len; ++i) {
        uint8_t byte = in[i];
        out[2 * i]     = hex_chars[(byte >> 4) & 0x0F];
        out[2 * i + 1] = hex_chars[byte & 0x0F];
    }

    out[in_len * 2] = '\0';
    return 0;
}

int web3c_hex_decode(const char *hex, uint8_t *out, size_t out_size) {
    if (hex == NULL || out == NULL) {
        return -1;
    }

    size_t len = 0;
    while (hex[len] != '\0') {
        ++len;
    }

    if (len % 2 != 0) {
        /* Hex string length must be even. */
        return -1;
    }

    size_t bytes_needed = len / 2;
    if (out_size < bytes_needed) {
        return -1;
    }

    for (size_t i = 0; i < bytes_needed; ++i) {
        int hi = hex_value(hex[2 * i]);
        int lo = hex_value(hex[2 * i + 1]);
        if (hi < 0 || lo < 0) {
            return -1;
        }
        out[i] = (uint8_t)((hi << 4) | lo);
    }

    return (int)bytes_needed;
}
