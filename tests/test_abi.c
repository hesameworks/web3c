#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "web3c/web3c.h"

static void test_uint256_basic(void) {
    unsigned char out[WEB3C_ABI_WORD_SIZE];
    int rc = web3c_abi_encode_uint256(1, out);
    assert(rc == 0);

    /* All bytes except the last one should be zero. */
    for (int i = 0; i < WEB3C_ABI_WORD_SIZE - 1; ++i) {
        assert(out[i] == 0);
    }
    assert(out[WEB3C_ABI_WORD_SIZE - 1] == 0x01);
}

static void test_address_basic(void) {
    unsigned char out[WEB3C_ABI_WORD_SIZE];
    unsigned char addr[20];

    memset(addr, 0xAB, sizeof(addr));

    int rc = web3c_abi_encode_address(addr, out);
    assert(rc == 0);

    /* First 12 bytes should be zero. */
    for (int i = 0; i < WEB3C_ABI_WORD_SIZE - 20; ++i) {
        assert(out[i] == 0);
    }

    /* Last 20 bytes should match the address. */
    assert(memcmp(out + (WEB3C_ABI_WORD_SIZE - 20), addr, 20) == 0);
}

static void test_bool_basic(void) {
    unsigned char out[WEB3C_ABI_WORD_SIZE];

    /* false -> all zeros */
    int rc = web3c_abi_encode_bool(0, out);
    assert(rc == 0);
    for (int i = 0; i < WEB3C_ABI_WORD_SIZE; ++i) {
        assert(out[i] == 0);
    }

    /* true -> last byte = 1, rest = 0 */
    rc = web3c_abi_encode_bool(1, out);
    assert(rc == 0);
    for (int i = 0; i < WEB3C_ABI_WORD_SIZE - 1; ++i) {
        assert(out[i] == 0);
    }
    assert(out[WEB3C_ABI_WORD_SIZE - 1] == 0x01);

    /* non-zero value should also be treated as true */
    rc = web3c_abi_encode_bool(42, out);
    assert(rc == 0);
    for (int i = 0; i < WEB3C_ABI_WORD_SIZE - 1; ++i) {
        assert(out[i] == 0);
    }
    assert(out[WEB3C_ABI_WORD_SIZE - 1] == 0x01);
}

static void test_bytes32_basic(void) {
    unsigned char out[WEB3C_ABI_WORD_SIZE];
    unsigned char val[WEB3C_ABI_WORD_SIZE];

    /* Fill val with a pattern and expect an exact copy. */
    for (int i = 0; i < WEB3C_ABI_WORD_SIZE; ++i) {
        val[i] = (unsigned char)(i & 0xFF);
    }

    int rc = web3c_abi_encode_bytes32(val, out);
    assert(rc == 0);

    assert(memcmp(out, val, WEB3C_ABI_WORD_SIZE) == 0);
}

int main(void) {
    printf("Running Web3C ABI tests...\n");

    test_uint256_basic();
    test_address_basic();
    test_bool_basic();
    test_bytes32_basic();

    printf("All tests passed.\n");
    return 0;
}
