#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

/*
 * Small demo: construct a legacy Ethereum transaction in memory
 * and print a human-readable summary.
 */

static void print_hex_addr(const uint8_t addr[20]) {
    char buf[20 * 2 + 1];

    if (web3c_hex_encode(addr, 20, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "hex encode failed\n");
        return;
    }

    printf("0x%s", buf);
}

int main(void) {
    web3c_tx_legacy tx;
    uint8_t to[20];

    /* Fill a dummy address. */
    for (int i = 0; i < 20; ++i) {
        to[i] = (uint8_t)(0x11 + i);
    }

    web3c_tx_legacy_init(&tx);

    tx.nonce     = 1;
    tx.gas_price = 20ULL * 1000000000ULL;      /* 20 gwei, in wei (for demo). */
    tx.gas_limit = 21000;
    tx.value     = 1000000000000000000ULL;     /* 1 ETH in wei (for demo). */
    tx.chain_id  = 1;                          /* Ethereum mainnet. */

    if (web3c_tx_legacy_set_to(&tx, to) != 0) {
        fprintf(stderr, "failed to set recipient\n");
        return 1;
    }

    /* No call data for a simple ETH transfer. */
    if (web3c_tx_legacy_set_data(&tx, NULL, 0) != 0) {
        fprintf(stderr, "failed to clear data\n");
        return 1;
    }

    if (web3c_tx_legacy_validate(&tx) != 0) {
        fprintf(stderr, "transaction is not valid\n");
        return 1;
    }

    printf("Legacy transaction demo:\n");
    printf("  nonce:      %llu\n", (unsigned long long)tx.nonce);
    printf("  chain_id:   %llu\n", (unsigned long long)tx.chain_id);
    printf("  gas_price:  %llu wei\n", (unsigned long long)tx.gas_price);
    printf("  gas_limit:  %llu\n", (unsigned long long)tx.gas_limit);
    printf("  value:      %llu wei\n", (unsigned long long)tx.value);

    printf("  to:         ");
    if (tx.has_to) {
        print_hex_addr(tx.to);
        printf("\n");
    } else {
        printf("<contract creation>\n");
    }

    printf("  data_len:   %zu bytes\n", tx.data_len);

    printf("Transaction struct looks valid (no RLP yet).\n");

    return 0;
}