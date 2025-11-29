#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "web3c/web3c.h"

static void test_tx_init(void) {
    web3c_tx_legacy tx;

    web3c_tx_legacy_init(&tx);

    assert(tx.nonce == 0);
    assert(tx.gas_price == 0);
    assert(tx.gas_limit == 0);
    assert(tx.value == 0);
    assert(tx.chain_id == 0);

    assert(tx.has_to == 0);

    for (int i = 0; i < 20; ++i) {
        assert(tx.to[i] == 0);
    }

    assert(tx.data == NULL);
    assert(tx.data_len == 0);
}

static void test_tx_set_to(void) {
    web3c_tx_legacy tx;
    uint8_t addr[20];

    for (int i = 0; i < 20; ++i) {
        addr[i] = (uint8_t)i;
    }

    web3c_tx_legacy_init(&tx);

    int rc = web3c_tx_legacy_set_to(&tx, addr);
    assert(rc == 0);
    assert(tx.has_to == 1);
    assert(memcmp(tx.to, addr, 20) == 0);
}

static void test_tx_set_data(void) {
    web3c_tx_legacy tx;
    uint8_t data[3] = { 0xde, 0xad, 0xbe };

    web3c_tx_legacy_init(&tx);

    int rc = web3c_tx_legacy_set_data(&tx, data, sizeof(data));
    assert(rc == 0);
    assert(tx.data == data);
    assert(tx.data_len == sizeof(data));

    /* Setting zero-length data with NULL pointer should be allowed. */
    rc = web3c_tx_legacy_set_data(&tx, NULL, 0);
    assert(rc == 0);
    assert(tx.data == NULL);
    assert(tx.data_len == 0);
}

static void test_tx_validate(void) {
    web3c_tx_legacy tx;
    web3c_tx_legacy_init(&tx);

    /* No chain_id, no gas_limit -> invalid. */
    assert(web3c_tx_legacy_validate(&tx) != 0);

    tx.chain_id = 1;
    /* Still invalid because gas_limit is zero. */
    assert(web3c_tx_legacy_validate(&tx) != 0);

    tx.gas_limit = 21000;
    /* Now should be valid (no data, no to required). */
    assert(web3c_tx_legacy_validate(&tx) == 0);

    /* Make it invalid again by setting data_len > 0 but data == NULL. */
    tx.data = NULL;
    tx.data_len = 10;
    assert(web3c_tx_legacy_validate(&tx) != 0);
}

int main(void) {
    printf("Running Web3C tx tests...\n");

    test_tx_init();
    test_tx_set_to();
    test_tx_set_data();
    test_tx_validate();

    printf("All tx tests passed.\n");
    return 0;
}