
# Web3C Architecture

## 1. Overview

Web3C is a lightweight C library that provides low-level, offline-friendly primitives for interacting with Ethereum-like (EVM-based) blockchains.

Main goals:

- Predictable and simple API for low-level programming
- No hidden allocations – caller owns the memory
- Offline-first design
- Security-oriented design

Use cases include:

- CLI tools
- Lightweight transaction builders
- Research tools
- Embedded / offline Web3 utilities

---

## 2. High-level Modules

The library is split into clean, focused modules:

- **abi**  
  ABI encoding for Solidity primitive types and function selectors.

- **hex**  
  Binary ↔ hex conversions.

- **keccak**  
  Ethereum-compatible Keccak-256 hashing (one-shot + streaming).

- **tx**  
  Legacy Ethereum transaction struct + validation + RLP preimage encoding.

- **rlp**  
  Minimal RLP encoder used by the tx module and exposed publicly.

- **rpc (planned)**  
  Optional lightweight JSON-RPC utilities.

Public headers live in:

include/web3c/

Users include:

#include <web3c/web3c.h>

---

## 3. ABI Module

### Goal

Minimal, dependency-free implementation of core ABI encoding.

### Implemented Types

- uint256 (from uint64)
- address (20-byte)
- bool
- bytes32
- bytes (dynamic: length → data → padding)

### Function Selectors

First 4 bytes of:

keccak256("functionSignature")

Example:

transfer(address,uint256) → a9059cbb

### Error Handling

- 0 = success  
- non-zero = error

No allocation inside library. Caller must allocate buffers.

---

## 4. Hex Module

Helpers used widely across Web3C:

- hex_encode  
  binary → lowercase hex  
- hex_decode  
  hex → binary  

Decode returns -1 on invalid input.

Always requires explicit buffer sizes for safety and static analysis.

---

## 5. Keccak-256 Module

### Goals

- Ethereum-compatible Keccak-256  
- No dependencies  
- Streaming + one-shot  
- Fully deterministic  

### API Summary

- web3c_keccak_ctx
- init
- update
- final
- web3c_keccak256() (one-shot)

### Internal Notes

- Uses Keccak-f1600 permutation  
- pad10*1 padding (Ethereum style)  
- Rate = 136 bytes  
- Portable and testable  

---

## 6. Transaction Module (Legacy)

### Scope

Implements:

- Legacy tx struct  
- Basic validation  
- RLP encoding of unsigned EIP-155 preimage  

Used for:

- Offline tx builders  
- Signing preimage hashing  
- Research tooling  

### Struct Layout

nonce  
gas_price  
gas_limit  
value  
chain_id  
has_to  
to[20]  
data pointer + length  

### Helpers

- init(tx)
- set_to(tx, addr20)
- set_data(tx, ptr, len)
- validate(tx)

Validation rules:

- chain_id != 0  
- gas_limit != 0  
- data_len > 0 → data != NULL  

### RLP Unsigned Preimage

List:

[ nonce, gasPrice, gasLimit, to, value, data, chainId, 0, 0 ]

API:

- rlp_size(tx, &out)
- rlp_encode(tx, out, out_size, &written)

Used for:

- Keccak hashing  
- secp256k1 signing  

---

## 7. RLP Module

### Goals

- Minimal  
- No recursion  
- No allocations  
- Size-only mode  
- Tx-ready primitives  

### Exposed primitives

- uint64 → RLP string
- bytes  → RLP string
- list header  

Rules:

- 0 → 0x80  
- 1-byte <= 0x7f → single byte  
- bytes len=0 → 0x80  
- list header encodes only header, not payload  

---

## 8. Design Principles

C-first, bindings-friendly  
No global state  
Security-oriented  
Minimal but composable  

---

## 9. Testing Strategy

Tests:

- ABI  
- Keccak  
- Selectors  
- Tx  
- RLP  

make test runs all tests.

Future:

- ABI vectors  
- Fuzz  
- CI  

---

## 10. Status

Modules implemented:

abi, hex, keccak, tx (unsigned), rlp

Experimental API.

Planned:

- ABI arrays/string  
- Signed tx  
- EIP-1559  
- RPC  
- CLI tools  