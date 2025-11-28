#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

/*
 * Build calldata for a hypothetical function:
 *
 *   setData(bytes data)
 *
 * Layout:
 *   4 bytes: selector = keccak256("setData(bytes)")[0:4]
 *   32 bytes: offset to data (0x20 = 32)
 *   32 bytes: length
 *   N bytes:  data
 *   padding:  zeros up to multiple of 32
 */

static void print_hex_prefixed(const uint8_t *data, size_t len) {
    /* Allocate enough for hex + null terminator. */
    char buf[4 + WEB3C_ABI_WORD_SIZE * 4 * 2]; /* plenty for demo */
    if (web3c_hex_encode(data, len, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "hex encode failed\n");
        return;
    }
    printf("0x%s\n", buf);
}

int main(void) {
    /* Example payload */
    const char *msg = "hello web3c";
    const uint8_t *payload = (const uint8_t *)msg;
    size_t payload_len = strlen(msg);

    /* 1) Compute selector for setData(bytes) */
    uint8_t selector[4];
    if (web3c_abi_function_selector("setData(bytes)", selector) != 0) {
        fprintf(stderr, "web3c_abi_function_selector failed\n");
        return 1;
    }

    /* 2) Encode head: single dynamic argument -> offset = 32 (0x20) */
    uint8_t head[WEB3C_ABI_WORD_SIZE];
    if (web3c_abi_encode_uint256(32, head) != 0) {
        fprintf(stderr, "web3c_abi_encode_uint256 failed\n");
        return 1;
    }

    /* 3) Encode tail (length + padded data) */
    uint8_t tail[WEB3C_ABI_WORD_SIZE * 3]; /* enough for small demo payloads */
    size_t tail_len = 0;

    if (web3c_abi_encode_bytes(payload, payload_len,
                               tail, sizeof(tail), &tail_len) != 0) {
        fprintf(stderr, "web3c_abi_encode_bytes failed\n");
        return 1;
    }

    /* 4) Build full calldata: selector (4) + head (32) + tail */
    uint8_t calldata[4 + WEB3C_ABI_WORD_SIZE * 4];
    size_t calldata_len = 0;

    memcpy(calldata, selector, 4);
    memcpy(calldata + 4, head, WEB3C_ABI_WORD_SIZE);
    memcpy(calldata + 4 + WEB3C_ABI_WORD_SIZE, tail, tail_len);

    calldata_len = 4 + WEB3C_ABI_WORD_SIZE + tail_len;

    printf("setData(bytes) calldata for \"%s\":\n  ", msg);
    print_hex_prefixed(calldata, calldata_len);

    return 0;
}