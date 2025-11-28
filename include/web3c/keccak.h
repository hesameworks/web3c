#ifndef WEB3C_KECCAK_H
#define WEB3C_KECCAK_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Keccak-256 context for streaming API.
 *
 * This context implements the Ethereum-style Keccak-256 hash function,
 * sometimes referred to as "Ethereum-SHA3".
 */
typedef struct {
    uint64_t state[25];   /* 1600-bit internal state */
    uint8_t  buffer[200]; /* temporary buffer for absorbing */
    size_t   rate;        /* rate in bytes (1088 bits => 136 bytes) */
    size_t   buffer_pos;  /* number of bytes currently in buffer */
    int      finalized;   /* flag to prevent further writes after final */
} web3c_keccak_ctx;

/**
 * @brief Initialize a Keccak-256 hashing context.
 */
void web3c_keccak256_init(web3c_keccak_ctx *ctx);

/**
 * @brief Absorb more data into the Keccak-256 context.
 *
 * @param ctx   Pointer to an initialized context.
 * @param data  Pointer to input bytes.
 * @param len   Number of input bytes.
 */
void web3c_keccak_update(web3c_keccak_ctx *ctx,
                         const uint8_t *data,
                         size_t len);

/**
 * @brief Finalize the Keccak-256 hash and write 32-byte digest.
 *
 * After calling this function the context must not be reused.
 *
 * @param ctx  Pointer to an initialized context.
 * @param out  Pointer to a 32-byte output buffer.
 */
void web3c_keccak_final(web3c_keccak_ctx *ctx, uint8_t out[32]);

/**
 * @brief Convenience one-shot Keccak-256 helper.
 *
 * @param data  Pointer to input bytes.
 * @param len   Number of bytes in @p data.
 * @param out   Pointer to a 32-byte output buffer.
 * @return 0 on success, non-zero on error.
 */
int web3c_keccak256(const uint8_t *data, size_t len, uint8_t out[32]);

#ifdef __cplusplus
}
#endif

#endif /* WEB3C_KECCAK_H */