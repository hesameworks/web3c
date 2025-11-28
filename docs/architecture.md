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

- `keccak` (planned)  
  Keccak-256 hashing for:
  - Computing Ethereum function selectors.
  - Address derivation and integrity checks.

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

- web3c_abi_encode_uint256(uint64_t value, unsigned char *out)

    - Encodes a 64-bit integer into a 32-byte ABI word.

    - Big-endian representation, value is placed in the last 8 bytes.

    - The buffer must have at least 32 bytes.

- web3c_abi_encode_address(const unsigned char *address, unsigned char *out)

    - Encodes a 20-byte Ethereum address into a 32-byte ABI word.

    - The address is right-aligned (last 20 bytes), left padded with zeros.

### 3.3 Error Handling

Both functions return:

- 0 on success.

- Non-zero on error (e.g. NULL pointer).

The library does not allocate memory internally. The caller is always responsible for:

- Allocating buffers of the correct size.

- Checking return codes and handling errors.

In the future, we may introduce a small web3c_error.h with well-defined
error codes instead of generic non-zero values.

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

Design choice: we explicitly require the caller to pass out_size to avoid
buffer overflows and to make static analysis easier.

## 5. Future Modules (Roadmap)
### 5.1 Keccak-256

Planned features:
- Pure C implementation with no external dependencies.
- API such as:
```c
int web3c_keccak256(const uint8_t *in, size_t in_len, uint8_t out[32]);
```
- Used for:
    - web3c_abi_function_selector("transfer(address,uint256)")
    - Hashing arbitrary payloads.

### 5.2 Function Selectors and ABI Helpers

On top of the core primitives:
- String → selector helpers.
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
### 5.3 Transaction Builder
A small tx module to:
- Build, sign (optionally) and serialize Ethereum transactions.
- Separate layers:
    - Raw transaction structure (nonce, gas, to, value, data).
    - RLP encoding.
    - Signature handling.

This is especially useful for offline tools that want to prepare raw
transactions to be signed by hardware wallets or air-gapped systems.

## 6. Design Principles
### 1.C-first, but bindings-friendly
The API is designed so that later it can be wrapped easily by:
- Python
- Rust
- Go or used from other languages via FFI.

### 2.No global state
All functions operate purely on inputs and outputs. This makes it easier to embed Web3C into multi-threaded or sandboxed environments.

### 3.Security-friendly layout
Small, explicit functions with fully documented preconditions. The goal is to make code reviews, fuzzing and formal analysis easier in the future.

### 3.Small surface area
Prefer a small but well-designed API that can be composed, instead of a huge, complicated one.

## 7. Testing Strategy
- Unit tests live under tests/.
- make test builds the static library and runs all tests.
- Future work:
    - Add more ABI test vectors based on official Ethereum ABI examples.
    - Add fuzzing harnesses (e.g. using libFuzzer or AFL++).
    - Add CI integration to run tests on every commit.

## 8. Status
This document reflects Phase 1 of the project.
The API surface is not stable yet and may change as more ABI types, Keccak-256 and transaction builders are introduced.