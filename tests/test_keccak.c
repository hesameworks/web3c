#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "web3c/keccak.h"

/* Simple hex encoder for tests */
static void web3c_bytes_to_hex(const uint8_t *in, size_t len, char *out)
{
    static const char *hex = "0123456789abcdef";

    for (size_t i = 0; i < len; ++i) {
        out[2 * i]     = hex[in[i] >> 4];
        out[2 * i + 1] = hex[in[i] & 0x0F];
    }
    out[2 * len] = '\0';
}

int main(void)
{
    uint8_t digest[32];
    char    hex[65];

    /* Ethereum keccak256("") test vector:
     * c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470
     */
    const char *expected_empty =
        "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470";

    if (web3c_keccak256((const uint8_t *)"", 0, digest) != 0) {
        fprintf(stderr, "web3c_keccak256() returned error\n");
        return 1;
    }

    web3c_bytes_to_hex(digest, sizeof(digest), hex);

    if (strcmp(hex, expected_empty) != 0) {
        fprintf(stderr,
                "Keccak-256(\"\") mismatch:\n  got:      %s\n  expected: %s\n",
                hex, expected_empty);
        return 1;
    }

    printf("Keccak-256 test passed.\n");
    return 0;
}