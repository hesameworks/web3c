#ifndef WEB3C_TX_H
#define WEB3C_TX_H

#include <stdint.h>
#include <stddef.h>

/*
 * Transaction primitives for Ethereum-like (EVM) chains.
 *
 * This module currently focuses on a legacy (pre-EIP-1559) transaction
 * representation, without any RLP encoding. The goal is to provide a
 * small, explicit struct that tooling can work with and validate.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Legacy Ethereum transaction (pre-EIP-1559).
 *
 * All numeric fields are currently represented as uint64_t for simplicity.
 * This is sufficient for many tools and testnets, but not a full 256-bit
 * representation. Future versions may introduce a dedicated 256-bit type.
 */
typedef struct {
    uint64_t nonce;      /* Transaction count for the sender address. */
    uint64_t gas_price;  /* Price per gas unit (in wei). */
    uint64_t gas_limit;  /* Maximum gas allowed for this tx. */
    uint64_t value;      /* Amount of ETH to transfer (in wei). */

    uint64_t chain_id;   /* Chain ID (EIP-155). Zero means "unset". */

    int      has_to;     /* 0 = contract creation, 1 = call to address. */
    uint8_t  to[20];     /* Recipient address (ignored if has_to == 0). */

    const uint8_t *data; /* Pointer to call data (owned by caller). */
    size_t        data_len;
} web3c_tx_legacy;

/*
 * Initialize a legacy transaction struct with safe defaults.
 *
 * All numeric fields are set to zero, has_to is cleared and data pointer
 * is set to NULL.
 */
void web3c_tx_legacy_init(web3c_tx_legacy *tx);

/*
 * Set the recipient address for a legacy transaction.
 *
 * Parameters:
 *   tx - pointer to the transaction struct.
 *   to - pointer to a 20-byte address.
 *
 * Returns:
 *   0 on success, non-zero on error (e.g. NULL pointers).
 */
int web3c_tx_legacy_set_to(web3c_tx_legacy *tx, const uint8_t to[20]);

/*
 * Set the call data pointer for a legacy transaction.
 *
 * No copy is performed; the caller remains responsible for the lifetime
 * of the data buffer.
 *
 * Parameters:
 *   tx   - pointer to the transaction struct.
 *   data - pointer to data buffer (can be NULL if len == 0).
 *   len  - length of the data in bytes.
 *
 * Returns:
 *   0 on success, non-zero on error (e.g. NULL tx, data == NULL && len > 0).
 */
int web3c_tx_legacy_set_data(web3c_tx_legacy *tx,
                             const uint8_t *data,
                             size_t len);

/*
 * Validate a legacy transaction for basic sanity.
 *
 * This does NOT perform any cryptographic checks or RLP validation.
 * It only enforces minimal conditions that make the struct usable:
 *
 *   - tx must not be NULL.
 *   - chain_id must be non-zero.
 *   - gas_limit must be non-zero.
 *   - if data_len > 0, data must not be NULL.
 *
 * Returns:
 *   0 if the transaction looks valid, non-zero otherwise.
 */
int web3c_tx_legacy_validate(const web3c_tx_legacy *tx);

#ifdef __cplusplus
}
#endif

#endif /* WEB3C_TX_H */