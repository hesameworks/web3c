#include "web3c/tx.h"
#include "web3c/rlp.h"

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

int web3c_tx_legacy_rlp_size(const web3c_tx_legacy *tx, size_t *out_size) {
    if (tx == NULL || out_size == NULL) {
        return -1;
    }

    if (web3c_tx_legacy_validate(tx) != 0) {
        return -1;
    }

    size_t len_nonce      = 0;
    size_t len_gas_price  = 0;
    size_t len_gas_limit  = 0;
    size_t len_to         = 0;
    size_t len_value      = 0;
    size_t len_data       = 0;
    size_t len_chain_id   = 0;
    size_t len_zero_r     = 0;
    size_t len_zero_s     = 0;

    /* 1) Size of each element (size-only mode). */
    if (web3c_rlp_encode_uint64(tx->nonce, NULL, 0, &len_nonce) != 0) {
        return -1;
    }
    if (web3c_rlp_encode_uint64(tx->gas_price, NULL, 0, &len_gas_price) != 0) {
        return -1;
    }
    if (web3c_rlp_encode_uint64(tx->gas_limit, NULL, 0, &len_gas_limit) != 0) {
        return -1;
    }

    if (tx->has_to) {
        if (web3c_rlp_encode_bytes(tx->to, 20, NULL, 0, &len_to) != 0) {
            return -1;
        }
    } else {
        if (web3c_rlp_encode_bytes(NULL, 0, NULL, 0, &len_to) != 0) {
            return -1;
        }
    }

    if (web3c_rlp_encode_uint64(tx->value, NULL, 0, &len_value) != 0) {
        return -1;
    }

    if (web3c_rlp_encode_bytes(tx->data, tx->data_len, NULL, 0, &len_data) != 0) {
        return -1;
    }

    if (web3c_rlp_encode_uint64(tx->chain_id, NULL, 0, &len_chain_id) != 0) {
        return -1;
    }

    /* r = 0, s = 0 for unsigned preimage. */
    if (web3c_rlp_encode_uint64(0, NULL, 0, &len_zero_r) != 0) {
        return -1;
    }
    if (web3c_rlp_encode_uint64(0, NULL, 0, &len_zero_s) != 0) {
        return -1;
    }

    size_t payload_len =
        len_nonce +
        len_gas_price +
        len_gas_limit +
        len_to +
        len_value +
        len_data +
        len_chain_id +
        len_zero_r +
        len_zero_s;

    size_t list_header_len = 0;
    if (web3c_rlp_encode_list_header(payload_len,
                                     NULL, 0, &list_header_len) != 0) {
        return -1;
    }

    *out_size = list_header_len + payload_len;
    return 0;
}

int web3c_tx_legacy_rlp_encode(const web3c_tx_legacy *tx,
                               uint8_t *out,
                               size_t out_size,
                               size_t *out_len)
{
    if (tx == NULL || out == NULL) {
        return -1;
    }

    size_t total_size = 0;
    if (web3c_tx_legacy_rlp_size(tx, &total_size) != 0) {
        return -1;
    }

    if (out_size < total_size) {
        return -1;
    }

    /* Recompute individual element sizes, then encode. */
    size_t len_nonce      = 0;
    size_t len_gas_price  = 0;
    size_t len_gas_limit  = 0;
    size_t len_to         = 0;
    size_t len_value      = 0;
    size_t len_data       = 0;
    size_t len_chain_id   = 0;
    size_t len_zero_r     = 0;
    size_t len_zero_s     = 0;

    if (web3c_rlp_encode_uint64(tx->nonce, NULL, 0, &len_nonce) != 0 ||
        web3c_rlp_encode_uint64(tx->gas_price, NULL, 0, &len_gas_price) != 0 ||
        web3c_rlp_encode_uint64(tx->gas_limit, NULL, 0, &len_gas_limit) != 0) {
        return -1;
    }

    if (tx->has_to) {
        if (web3c_rlp_encode_bytes(tx->to, 20, NULL, 0, &len_to) != 0) {
            return -1;
        }
    } else {
        if (web3c_rlp_encode_bytes(NULL, 0, NULL, 0, &len_to) != 0) {
            return -1;
        }
    }

    if (web3c_rlp_encode_uint64(tx->value, NULL, 0, &len_value) != 0 ||
        web3c_rlp_encode_bytes(tx->data, tx->data_len, NULL, 0, &len_data) != 0 ||
        web3c_rlp_encode_uint64(tx->chain_id, NULL, 0, &len_chain_id) != 0 ||
        web3c_rlp_encode_uint64(0, NULL, 0, &len_zero_r) != 0 ||
        web3c_rlp_encode_uint64(0, NULL, 0, &len_zero_s) != 0) {
        return -1;
    }

    size_t payload_len =
        len_nonce +
        len_gas_price +
        len_gas_limit +
        len_to +
        len_value +
        len_data +
        len_chain_id +
        len_zero_r +
        len_zero_s;

    size_t list_header_len = 0;
    if (web3c_rlp_encode_list_header(payload_len,
                                     out, out_size, &list_header_len) != 0) {
        return -1;
    }

    size_t offset = list_header_len;

    /* Now encode each element into place. */
    if (web3c_rlp_encode_uint64(tx->nonce,
                                out + offset, out_size - offset, &len_nonce) != 0) {
        return -1;
    }
    offset += len_nonce;

    if (web3c_rlp_encode_uint64(tx->gas_price,
                                out + offset, out_size - offset, &len_gas_price) != 0) {
        return -1;
    }
    offset += len_gas_price;

    if (web3c_rlp_encode_uint64(tx->gas_limit,
                                out + offset, out_size - offset, &len_gas_limit) != 0) {
        return -1;
    }
    offset += len_gas_limit;

    if (tx->has_to) {
        if (web3c_rlp_encode_bytes(tx->to, 20,
                                   out + offset, out_size - offset, &len_to) != 0) {
            return -1;
        }
    } else {
        if (web3c_rlp_encode_bytes(NULL, 0,
                                   out + offset, out_size - offset, &len_to) != 0) {
            return -1;
        }
    }
    offset += len_to;

    if (web3c_rlp_encode_uint64(tx->value,
                                out + offset, out_size - offset, &len_value) != 0) {
        return -1;
    }
    offset += len_value;

    if (web3c_rlp_encode_bytes(tx->data, tx->data_len,
                               out + offset, out_size - offset, &len_data) != 0) {
        return -1;
    }
    offset += len_data;

    if (web3c_rlp_encode_uint64(tx->chain_id,
                                out + offset, out_size - offset, &len_chain_id) != 0) {
        return -1;
    }
    offset += len_chain_id;

    if (web3c_rlp_encode_uint64(0,
                                out + offset, out_size - offset, &len_zero_r) != 0) {
        return -1;
    }
    offset += len_zero_r;

    if (web3c_rlp_encode_uint64(0,
                                out + offset, out_size - offset, &len_zero_s) != 0) {
        return -1;
    }
    offset += len_zero_s;

    if (out_len) {
        *out_len = offset;
    }

    return 0;
}