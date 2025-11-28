#ifndef WEB3C_HEX_H
#define WEB3C_HEX_H

#include <stddef.h>
#include <stdint.h>

/*
 * Simple hex encoding/decoding utilities.
 *
 * These are used internally by the library but are also useful
 * for command-line tools and examples.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Convert a binary buffer to a lowercase hex string.
 *
 * Parameters:
 *   in       - pointer to input bytes
 *   in_len   - number of input bytes
 *   out      - pointer to output buffer for hex chars
 *   out_size - size of the output buffer in bytes
 *
 * The output buffer must have size at least (in_len * 2 + 1)
 * to hold the hex representation plus a null terminator.
 *
 * Returns:
 *   0 on success, non-zero on error (e.g. buffer too small).
 */
int web3c_hex_encode(const uint8_t *in, size_t in_len, char *out, size_t out_size);

/*
 * Decode a hex string into binary.
 *
 * Parameters:
 *   hex      - null-terminated hex string (uppercase or lowercase)
 *   out      - pointer to output buffer for bytes
 *   out_size - size of the output buffer in bytes
 *
 * Returns:
 *   number of bytes written on success,
 *   -1 on error (invalid length or invalid characters).
 */
int web3c_hex_decode(const char *hex, uint8_t *out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* WEB3C_HEX_H */
