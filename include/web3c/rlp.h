#ifndef WEB3C_RLP_H
#define WEB3C_RLP_H

#include <stdint.h>
#include <stddef.h>

/*
 * Minimal RLP encoder for uint64, byte strings, and list headers.
 *
 * All functions support "size-only" mode:
 *   - If out == NULL, no bytes are written, but out_len (if non-NULL)
 *     receives the required size.
 *   - If out != NULL, out_size must be >= required size.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Encode a uint64_t as an RLP "string" (big-endian, no leading zeros).
 *
 * Special cases:
 *   - value == 0 is encoded as the empty string: 0x80
 *   - 0x00 < value <= 0x7f is encoded as a single byte: [value]
 *
 * Parameters:
 *   value    - integer to encode.
 *   out      - output buffer (can be NULL for size-only mode).
 *   out_size - size of the output buffer in bytes.
 *   out_len  - if non-NULL, receives the number of bytes written.
 *
 * Returns:
 *   0 on success, non-zero on error.
 */
int web3c_rlp_encode_uint64(uint64_t value,
                            uint8_t *out,
                            size_t out_size,
                            size_t *out_len);

/*
 * Encode an arbitrary byte string as RLP.
 *
 * For len == 0, encode as the empty string (0x80).
 * For len == 1 and data[0] <= 0x7f, encode as a single byte.
 * For other lengths, follow the RLP string rules.
 *
 * Parameters:
 *   data     - pointer to bytes (can be NULL if len == 0).
 *   len      - number of bytes.
 *   out      - output buffer (can be NULL for size-only mode).
 *   out_size - size of the output buffer in bytes.
 *   out_len  - if non-NULL, receives the number of bytes written.
 *
 * Returns:
 *   0 on success, non-zero on error.
 */
int web3c_rlp_encode_bytes(const uint8_t *data,
                           size_t len,
                           uint8_t *out,
                           size_t out_size,
                           size_t *out_len);

/*
 * Encode a list header given the payload length in bytes.
 *
 * The payload is the concatenation of already RLP-encoded elements.
 *
 * Parameters:
 *   payload_len - length of the list payload in bytes.
 *   out         - output buffer (can be NULL for size-only mode).
 *   out_size    - size of the output buffer in bytes.
 *   out_len     - if non-NULL, receives the number of bytes written.
 *
 * Returns:
 *   0 on success, non-zero on error.
 */
int web3c_rlp_encode_list_header(size_t payload_len,
                                 uint8_t *out,
                                 size_t out_size,
                                 size_t *out_len);

#ifdef __cplusplus
}
#endif

#endif /* WEB3C_RLP_H */