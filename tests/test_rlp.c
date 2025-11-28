#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

static void test_rlp_uint_basic(void) {
    uint8_t buf[16];
    size_t len = 0;

    /* 0 -> 0x80 (empty string) */
    assert(web3c_rlp_encode_uint64(0, buf, sizeof(buf), &len) == 0);
    assert(len == 1);
    assert(buf[0] == 0x80);

    /* 15 -> single byte 0x0f */
    assert(web3c_rlp_encode_uint64(15, buf, sizeof(buf), &len) == 0);
    assert(len == 1);
    assert(buf[0] == 0x0f);

    /* 1024 -> 0x82 0x04 0x00 */
    assert(web3c_rlp_encode_uint64(1024, buf, sizeof(buf), &len) == 0);
    assert(len == 3);
    assert(buf[0] == 0x82);
    assert(buf[1] == 0x04);
    assert(buf[2] == 0x00);
}

static void test_rlp_bytes_basic(void) {
    uint8_t buf[32];
    size_t len = 0;

    /* Empty string -> 0x80 */
    assert(web3c_rlp_encode_bytes(NULL, 0, buf, sizeof(buf), &len) == 0);
    assert(len == 1);
    assert(buf[0] == 0x80);

    /* Single byte <= 0x7f -> itself */
    uint8_t b = 0x7f;
    assert(web3c_rlp_encode_bytes(&b, 1, buf, sizeof(buf), &len) == 0);
    assert(len == 1);
    assert(buf[0] == 0x7f);

    /* "cat" -> 0x83 'c' 'a' 't' */
    const uint8_t cat[] = { 'c', 'a', 't' };
    assert(web3c_rlp_encode_bytes(cat, sizeof(cat), buf, sizeof(buf), &len) == 0);
    assert(len == 4);
    assert(buf[0] == 0x83);
    assert(buf[1] == 'c');
    assert(buf[2] == 'a');
    assert(buf[3] == 't');
}

static void test_rlp_list_header_basic(void) {
    uint8_t buf[16];
    size_t len = 0;

    /* Payload length 3 -> 0xC3 */
    assert(web3c_rlp_encode_list_header(3, buf, sizeof(buf), &len) == 0);
    assert(len == 1);
    assert(buf[0] == 0xC3);

    /* Payload length 60 -> 0xF8 0x3C */
    assert(web3c_rlp_encode_list_header(60, buf, sizeof(buf), &len) == 0);
    assert(len == 2);
    assert(buf[0] == 0xF8);
    assert(buf[1] == 0x3C);
}

int main(void) {
    printf("Running Web3C RLP tests...\n");

    test_rlp_uint_basic();
    test_rlp_bytes_basic();
    test_rlp_list_header_basic();

    printf("All RLP tests passed.\n");
    return 0;
}