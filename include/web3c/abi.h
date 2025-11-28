#ifndef WEB3C_ABI_H
#define WEB3C_ABI_H

#include <stdint.h>
#include <stddef.h>

/*
 * ABI encoding helpers for a subset of Ethereum types.
 *
 * The goal of this module is to provide simple, predictable
 * functions that can be used in low-level, offline tools,
 * without any dynamic allocation.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Size of a single ABI word in bytes (32 bytes). */
#define WEB3C_ABI_WORD_SIZE 32

/*
 * Encode an unsigned 256-bit integer into a 32-byte ABI word.
 *
 * For now, this function accepts a uint64_t and left-pads it
 * inside the 256-bit word (big-endian representation).
 *
 * Parameters:
 *   value - 64-bit unsigned integer to encode.
 *   out   - pointer to a buffer of at least 32 bytes.
 *
 * Returns:
 *   0 on success, non-zero on error (e.g. NULL pointer).
 */
int web3c_abi_encode_uint256(uint64_t value, unsigned char *out);

/*
 * Encode an Ethereum address (20 bytes) into a 32-byte ABI word.
 *
 * The address is right-aligned and left-padded with zeros as per ABI spec.
 *
 * Parameters:
 *   address - pointer to a buffer of exactly 20 bytes.
 *   out     - pointer to a buffer of at least 32 bytes.
 *
 * Returns:
 *   0 on success, non-zero on error.
 */
int web3c_abi_encode_address(const unsigned char *address, unsigned char *out);

/*
 * Encode a Solidity bool into a 32-byte ABI word.
 *
 * Any non-zero value is treated as true (1), zero as false (0).
 * The value is stored in the least significant byte of the word,
 * with all other bytes set to zero.
 *
 * Parameters:
 *   value - integer value to interpret as bool.
 *   out   - pointer to a buffer of at least 32 bytes.
 *
 * Returns:
 *   0 on success, non-zero on error.
 */
int web3c_abi_encode_bool(int value, unsigned char *out);

/*
 * Encode a Solidity bytes32 value into a 32-byte ABI word.
 *
 * Parameters:
 *   value - pointer to a buffer of exactly 32 bytes.
 *   out   - pointer to a buffer of at least 32 bytes.
 *
 * Returns:
 *   0 on success, non-zero on error.
*/
int web3c_abi_encode_bytes32(const unsigned char *value, unsigned char *out);

/*
 * Encode a Solidity bytes value (dynamic length) into ABI format.
 *
 * Layout:
 *   - 32 bytes: length (uint256, big-endian)
 *   - N bytes:  actual data
 *   - padding with zeros up to a multiple of 32 bytes
 *
 * This helper encodes only the "tail" (length + padded data) for a single
 * bytes value. When used as a single argument to a function, the caller is
 * responsible for placing the correct offset (usually 0x20) in the head.
 *
 * Parameters:
 *   data     - pointer to input bytes (can be NULL if len == 0).
 *   len      - number of input bytes.
 *   out      - output buffer.
 *   out_size - size of the output buffer in bytes.
 *   out_len  - if non-NULL, receives the number of bytes written.
 *
 * Returns:
 *   0 on success, non-zero on error (e.g. NULL pointers, buffer too small).
 */
int web3c_abi_encode_bytes(const uint8_t *data,
    size_t len,
    unsigned char *out,
    size_t out_size,
    size_t *out_len);

/*
 * Compute the 4-byte function selector for a given Solidity signature.
 *
 * Example:
 *   signature = "transfer(address,uint256)"
 *   selector  = first 4 bytes of keccak256(signature)
 *
 * Parameters:
 *   signature - null-terminated ASCII function signature.
 *   out       - pointer to a 4-byte buffer that receives the selector.
 *
 * Returns:
 *   0 on success, non-zero on error (e.g. NULL pointers).
*/
int web3c_abi_function_selector(const char *signature, unsigned char out[4]);

#ifdef __cplusplus
}
#endif

#endif /* WEB3C_ABI_H */
