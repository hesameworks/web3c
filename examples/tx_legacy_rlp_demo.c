#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "web3c/web3c.h"

static void print_hex(const uint8_t *data, size_t len) {
    char buf[2048];

    if (web3c_hex_encode(data, len, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "hex encode failed\n");
        return;
    }

    printf("0x%s\n", buf);
}

int main(void) {
    web3c_tx_legacy tx;
    uint8_t to[20];

    for (int i = 0; i < 20; ++i) {
        to[i] = (uint8_t)(0x11 + i);
    }

    web3c_tx_legacy_init(&tx);

    tx.nonce     = 1;
    tx.gas_price = 20ULL * 1000000000ULL;
    tx.gas_limit = 21000;
    tx.value     = 1000000000000000000ULL;
    tx.chain_id  = 1;

    if (web3c_tx_legacy_set_to(&tx, to) != 0) {
        fprintf(stderr, "failed to set recipient\n");
        return 1;
    }

    if (web3c_tx_legacy_set_data(&tx, NULL, 0) != 0) {
        fprintf(stderr, "failed to clear data\n");
        return 1;
    }

    if (web3c_tx_legacy_validate(&tx) != 0) {
        fprintf(stderr, "tx is not valid\n");
        return 1;
    }

    size_t rlp_size = 0;
    if (web3c_tx_legacy_rlp_size(&tx, &rlp_size) != 0) {
        fprintf(stderr, "failed to compute RLP size\n");
        return 1;
    }

    uint8_t *buf = (uint8_t *)malloc(rlp_size);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    size_t out_len = 0;
    if (web3c_tx_legacy_rlp_encode(&tx, buf, rlp_size, &out_len) != 0) {
        fprintf(stderr, "RLP encode failed\n");
        free(buf);
        return 1;
    }

    printf("Legacy tx RLP (unsigned preimage):\n  ");
    print_hex(buf, out_len);

    free(buf);
    return 0;
}