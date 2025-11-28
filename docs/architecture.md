# Web3C Architecture

## 1. Overview

Web3C is a lightweight C library that provides low-level, offline-friendly primitives for interacting with Ethereum-like (EVM-based) blockchains.

The main goals are:

- **Predictable and simple API** for systems programming and tooling.
- **No hidden allocations** – the caller owns the memory.
- **Offline-first design** – all core features can be used without network access.
- **Security-oriented mindset** – explicit handling of inputs, sizes and error codes.

This is not a full-featured Web3 SDK. Instead, it is a focused toolkit for building:

- Command-line utilities.
- Lightweight wallet and transaction tools.
- Testing / fuzzing harnesses and security analysis tools.

---

## 2. High-level Modules

The library is split into small modules:

- `abi`  
  Responsible for ABI encoding of static types (and later, dynamic types).

- `hex`  
  Binary ↔ hex helpers, used both internally and by examples / tools.

- `keccak`  
  Ethereum-style Keccak-256 hashing for:
  - Computing function selectors.
  - Hashing call data and arbitrary payloads.
  - Future address- and integrity-related helpers.

- `tx` / `tx_builder` (planned)  
  Primitives for constructing raw Ethereum transactions in RLP form.

- `rpc` (planned, optional)  
  Minimal JSON-RPC helpers for talking to Ethereum nodes. This will be
  designed as an **optional layer** on top of the core, so that offline
  tools do not depend on any JSON or networking code if they do not need it.

The public headers live under:

- `include/web3c/`  
  and users are expected to include:

```c
#include <web3c/web3c.h>
```

## 3. ABI Module Design (Phase 1)
### 3.1 Goals
- Provide explicit, minimal functions for encoding basic ABI types.
- Avoid dynamic memory allocation.
- Make the data layout easy to reason about for security reviews.

### 3.2 Current Scope

Implemented functions:

- `web3c_abi_encode_uint256(uint64_t value, unsigned char *out)`

  - Encodes a 64-bit integer into a 32-byte ABI word.
  - Big-endian representation, value is placed in the last 8 bytes.
  - The buffer must have at least 32 bytes.

- `web3c_abi_encode_address(const unsigned char *address, unsigned char *out)`

  - Encodes a 20-byte Ethereum address into a 32-byte ABI word.
  - The address is right-aligned (last 20 bytes), left padded with zeros.

- `web3c_abi_encode_bool(int value, unsigned char *out)`

  - Encodes a Solidity `bool` as a 32-byte ABI word.
  - `false` → all-zero word.
  - `true`  → last byte = `0x01`, all other bytes = `0x00`.
  - Any non-zero input value is treated as `true`.

- `web3c_abi_encode_bytes32(const unsigned char *value, unsigned char *out)`

  - Encodes a fixed-size 32-byte `bytes32` value.
  - Simply copies 32 bytes as-is into the ABI word.

- `web3c_abi_encode_bytes(const uint8_t *data,
                          size_t len,
                          unsigned char *out,
                          size_t out_size,
                          size_t *out_len)`

  - Encodes a dynamic `bytes` value as ABI tail:
    - 32-byte length field (`uint256`, big-endian).
    - Followed by the raw data.
    - Followed by zero-padding up to a multiple of 32 bytes.
  - Intended for the tail of a single-argument `bytes` parameter.

### 3.3 Error Handling
Both functions return:
- 0 on success.
- Non-zero on error (e.g. NULL pointer).
The library does not allocate memory internally. The caller is always responsible for:
- Allocating buffers of the correct size.
- Checking return codes and handling errors.
In the future, we may introduce a small web3c_error.h with well-defined error codes instead of generic non-zero values.

### 3.4 Function selectors

On top of the basic ABI primitives, Web3C provides a small helper for computing
4-byte function selectors used by Ethereum smart contracts:

```c
int web3c_abi_function_selector(const char *signature,
                                unsigned char out[4]);
```
- signature is a null-terminated Solidity function signature, e.g. "transfer(address,uint256)".
- The function computes keccak256(signature) using the Keccak module and copies the first 4 bytes into out.
This helper is useful for:
- Building calldata for contract calls.
- Security tooling that needs to analyze or generate function selectors.
- Minimal wallet / scripting utilities.

### 3.5 Dynamic bytes layout

Dynamic `bytes` in the Solidity ABI are encoded as:

- A 32-byte length field (`uint256`).
- The actual byte sequence.
- Zero-padding up to a multiple of 32 bytes.

In the case of a single `bytes` argument, the head contains a single
offset (usually `0x20`), and the tail is exactly:

- `length` word
- padded data

Web3C exposes this as:

```c
int web3c_abi_encode_bytes(const uint8_t *data,
                           size_t len,
                           unsigned char *out,
                           size_t out_size,
                           size_t *out_len);
```
The example examples/setdata_bytes_calldata demonstrates how to:
- Compute the setData(bytes) selector.
- Build the head with the correct offset.
- Append the encoded tail (length + padded payload).

## 4. Hex Module Design
The hex module exists because virtually every Web3 tooling flow needs:
- Converting binary data (ABI words, hashes, raw transactions) to hex strings.
- Parsing user input (addresses, transaction blobs) from hex.
Current functions:
- int web3c_hex_encode(const uint8_t *in, size_t in_len, char *out, size_t out_size);
  - Outputs lowercase hex.
  - Requires out_size >= in_len * 2 + 1.
  - Returns 0 on success, non-zero on error.
- int web3c_hex_decode(const char *hex, uint8_t *out, size_t out_size);
  - Accepts upper or lowercase hex.
  - Returns number of bytes written on success.
  - Returns -1 on error (invalid input length or invalid character).
Design choice: we explicitly require the caller to pass out_size to avoid buffer overflows and to make static analysis easier.

## 5. Keccak-256 Module Design
### 5.1 Goals
- Provide an Ethereum-compatible Keccak-256 implementation.
- Offer both a simple one-shot helper and a streaming API.
- Keep the implementation self-contained (pure C, no external deps).

### 5.2 API Surface
Public API (as of Phase 1):
```c
typedef struct {
    uint64_t state[25];   /* 1600-bit internal state */
    uint8_t  buffer[200]; /* temporary buffer for absorbing */
    size_t   rate;        /* rate in bytes (1088 bits => 136 bytes) */
    size_t   buffer_pos;  /* number of bytes currently in buffer */
    int      finalized;   /* flag to prevent further writes after final */
} web3c_keccak_ctx;

void web3c_keccak256_init(web3c_keccak_ctx *ctx);

void web3c_keccak_update(web3c_keccak_ctx *ctx,
                         const uint8_t *data,
                         size_t len);

void web3c_keccak_final(web3c_keccak_ctx *ctx, uint8_t out[32]);

int web3c_keccak256(const uint8_t *data, size_t len, uint8_t out[32]);
```
- The context type is fully defined in the public header to make it easy to allocate on the stack or embed inside other structs.
- The one-shot helper web3c_keccak256 is built on top of the streaming API.

### 5.3 Internal Design
- The implementation uses the Keccak-f[1600] permutation with:
  - 1600-bit internal state (25 * 64-bit lanes).
  - Rate of 1088 bits (136 bytes) and capacity of 512 bits.
- Padding follows Keccak pad10*1 in the Ethereum style ("Ethereum-SHA3"), which is what Solidity's keccak256 uses.
- The code operates on little-endian byte order for lane load/store helpers, making it portable across common architectures.
The module is designed to be:
- Easy to test against known Ethereum Keccak-256 vectors.
- Reusable for function selectors, event topics and any other hashing needs inside the library.

## 6. Future Modules (Roadmap)
### 6.1 Function Selectors and ABI Helpers
- On top of the core primitives: String -> selector helpers (e.g. computing the first 4 bytes of keccak256("transfer(address,uint256)")).
- Helpers to build full call data blobs, e.g.:
```c
// Pseudo-API for future phases
int web3c_abi_encode_call(
    const char *signature,
    const web3c_abi_value *args,
    size_t args_count,
    uint8_t *out,
    size_t out_size
);
```
### 6.2 Transaction Builder
A small tx module to:
- Build, sign (optionally) and serialize Ethereum transactions.
- Separate layers:
  - Raw transaction structure (nonce, gas, to, value, data).
  - RLP encoding.
  - Signature handling.
This is especially useful for offline tools that want to prepare raw transactions to be signed by hardware wallets or air-gapped systems.

## 7. Transaction module (legacy v1)

### 7.1 Scope

The `tx` module in Web3C focuses on a minimal, explicit representation of
legacy (pre-EIP-1559) Ethereum transactions. The goal is:

- Provide a small, dependency-free `struct` that can be used by
  offline tools, test harnesses, and educational projects.
- Avoid dynamic allocation inside the library – callers own all buffers.
- Keep numeric fields at `uint64_t` for now, which is sufficient for
  many testnets and local tools, while keeping the design open for a
  future 256-bit representation.

At this stage, the module does **not** perform RLP encoding or signing.
It only models the transaction in memory and provides basic validation.

### 7.2 Legacy struct layout

The core type is:

```c
typedef struct {
    uint64_t nonce;
    uint64_t gas_price;
    uint64_t gas_limit;
    uint64_t value;

    uint64_t chain_id;   /* Zero means "unset". */

    int      has_to;     /* 0 = contract creation, 1 = call to address. */
    uint8_t  to[20];     /* Recipient address (ignored if has_to == 0). */

    const uint8_t *data; /* Pointer to call data (owned by caller). */
    size_t        data_len;
} web3c_tx_legacy;
```
Design notes:
- All monetary and gas-related amounts are kept as uint64_t. This is a deliberate trade-off between simplicity and completeness. Many scripts and small tools never hit the upper bounds of 256-bit values.
- Contract creation is represented by has_to == 0. In that case the to field is ignored.
- Call data is represented as a (pointer, length) pair. The library does not allocate or free this memory.

### 7.3 Ownership model and helpers

The following helpers are provided:
- void web3c_tx_legacy_init(web3c_tx_legacy *tx);
  - Zero-initializes the struct and sets has_to = 0, data = NULL, data_len = 0.
  - Callers are expected to fill in the numeric fields explicitly.
- int web3c_tx_legacy_set_to(web3c_tx_legacy *tx, const uint8_t to[20]);
  - Copies a 20-byte address into tx->to and sets has_to = 1.
- int web3c_tx_legacy_set_data(web3c_tx_legacy *tx, const uint8_t *data, size_t len);
  - Sets the (pointer, length) pair for call data.
  - Does not copy the buffer; the caller retains ownership.
  - len == 0 with data == NULL is treated as “no data”.
- int web3c_tx_legacy_validate(const web3c_tx_legacy *tx);
  - Performs minimal sanity checks:
    - tx must not be NULL.
    - chain_id must be non-zero.
    - gas_limit must be non-zero.
    - If data_len > 0, then data must not be NULL.
- Returns 0 if the struct is usable, non-zero otherwise.
This keeps the module small but safe enough for higher-level tools to
build on top of (RLP encoding, signing, broadcasting).

### 7.4 Future: RLP and signing
The next planned steps for the tx module are:
- RLP size estimation and encoding for web3c_tx_legacy.
- A clear separation between:
  - “unsigned” transaction (fields only).
  - “signed” transaction (including v, r, s).
- Optional helpers for building RLP blobs that can be sent over JSON-RPC
(e.g. eth_sendRawTransaction).

## 8. Design Principles
### 1.C-first, but bindings-friendly
The API is designed so that later it can be wrapped easily by:
- Python
- Rust
- Go or used from other languages via FFI.
### 2.No global state
All functions operate purely on inputs and outputs. This makes it easier to embed Web3C into multi-threaded or sandboxed environments.
### 3.Security-friendly layout
Small, explicit functions with fully documented preconditions. The goal is to make code reviews, fuzzing and formal analysis easier in the future.
### 4.Small surface area
Prefer a small but well-designed API that can be composed, instead of a huge, complicated one.
## 9. Testing Strategy
- Unit tests live under tests/.
- make test builds the static library and runs all tests.
- Current coverage:
  - ABI encoding sanity tests.
  - Keccak-256 test vector(s) for known inputs.
- Future work:
  - Add more ABI test vectors based on official Ethereum ABI examples.
  - Add fuzzing harnesses (e.g. using libFuzzer or AFL++).
  - Add CI integration to run tests on every commit.
## 10. Status
This document reflects Phase 1 of the project.
The API surface is not stable yet and may change as more ABI types, Keccak-256 helpers and transaction builders are introduced.