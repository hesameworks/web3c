#include "web3c/tx.h"

#include <string.h>

void web3c_tx_legacy_init(web3c_tx_legacy *tx) {
    if (tx == NULL) {
        return;
    }

    memset(tx, 0, sizeof(*tx));

    /* Explicitly document that data is NULL and has_to is false. */
    tx->has_to  = 0;
    tx->data    = NULL;
    tx->data_len = 0;
}

int web3c_tx_legacy_set_to(web3c_tx_legacy *tx, const uint8_t to[20]) {
    if (tx == NULL || to == NULL) {
        return -1;
    }

    memcpy(tx->to, to, 20);
    tx->has_to = 1;

    return 0;
}

int web3c_tx_legacy_set_data(web3c_tx_legacy *tx,
                             const uint8_t *data,
                             size_t len)
{
    if (tx == NULL) {
        return -1;
    }

    if (len > 0 && data == NULL) {
        return -1;
    }

    tx->data     = data;
    tx->data_len = len;

    return 0;
}

int web3c_tx_legacy_validate(const web3c_tx_legacy *tx) {
    if (tx == NULL) {
        return -1;
    }

    if (tx->chain_id == 0) {
        /* Chain ID must be set for EIP-155 compatible signatures. */
        return -1;
    }

    if (tx->gas_limit == 0) {
        /* A zero gas limit is not meaningful. */
        return -1;
    }

    if (tx->data_len > 0 && tx->data == NULL) {
        /* Data length without a valid pointer is inconsistent. */
        return -1;
    }

    /* Note: has_to == 0 is allowed (contract creation). */
    return 0;
}