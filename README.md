# Web3C

Web3C is a lightweight C library providing low-level, offline-friendly primitives for interacting with Web3 / Ethereum-like blockchains.

> Status: Experimental – API is not stable yet.

---

## Features

### 🔹 ABI Encoding

Minimal ABI encoder for core Solidity types:

- `uint256` (from `uint64_t`)
- `address` (20-byte)
- `bool`
- `bytes32`
- `bytes` (dynamic, single-argument tail encoder)

Supported patterns:

- Single value encoding (e.g. `uint256` → 32-byte word)
- Static arguments in the head
- Dynamic `bytes` encoded in the tail (offset + length + data)

### 🔹 Function Selectors

- Compute 4-byte function selectors from Solidity signatures:
  - `"transfer(address,uint256)"` → `a9059cbb`
- Uses Ethereum-style Keccak-256 under the hood.

### 🔹 Hex Utilities

- Binary → hex (lowercase)
- Hex → binary
- Helpful for printing ABI, RLP, and hash outputs in CLI tools.

### 🔹 Keccak-256

Ethereum-style Keccak-256 hash function:

- One-shot API for simple uses
- Streaming API for large or incremental data

### 🔹 Legacy Transaction Primitives

A minimal, dependency-free representation of Ethereum legacy transactions:

- `web3c_tx_legacy` struct:
  - `nonce`
  - `gas_price`
  - `gas_limit`
  - `to` (20-byte address or empty for contract creation)
  - `value`
  - `chain_id`
  - `data` (pointer + length)
- Small helpers for:
  - Initialization
  - Setting fields
  - Basic validation (e.g. data pointer/length consistency)

### 🔹 RLP Encoding

A small RLP module, focused on primitives needed for Ethereum transactions:

- RLP for:
  - `uint64` (string form)
  - Raw byte strings
  - List headers (short & long)
- Size-only mode:
  - Pass `out == NULL` to only compute the required buffer size.
- Legacy transaction RLP (unsigned preimage):
  - Encodes `[nonce, gasPrice, gasLimit, to, value, data, chainId, 0, 0]`
  - Suitable as a preimage for:
    - Keccak-256 hashing
    - secp256k1 signing

---

## Motivation

Most Web3 tooling is written in high-level languages (TypeScript, Python, Rust). Web3C targets a different layer:

- Embedded systems
- Minimal command-line tools
- Security and research tooling
- Environments where a small, auditable C library is preferred over a large SDK

The goal is not to replace existing Web3 SDKs, but to provide a thin, predictable layer that can be embedded almost anywhere.

---

## Design Goals

- **Offline-first** – core features work without any network or JSON-RPC.
- **No dynamic allocation in core modules** – the caller controls all memory.
- **Small and explicit API** – easy to audit and reason about.
- **FFI & bindings friendly** – simple C types, no global state.
- **Deterministic** – no hidden I/O, no randomness, no external dependencies.

For more details, see [`docs/architecture.md`](docs/architecture.md).

---

## Examples

Small, self-contained examples live under the `examples/` directory:

- `examples/simple_encode`  
  Encode a `uint256` into a 32-byte ABI word and print it as hex.

- `examples/abi_types_demo`  
  Demonstrate ABI encoding for `uint256`, `bool`, `bytes32`, and `bytes` (dynamic) in a single program.

- `examples/erc20_transfer_calldata`  
  Build ERC-20 `transfer(address,uint256)` calldata from CLI arguments:
  ```bash
  ./examples/erc20_transfer_calldata 0x00112233445566778899aabbccddeeff00112233 1000
  ```
-`examples/setdata_bytes_calldata`
Build calldata for a hypothetical `setData(bytes)` function, showing how dynamic `bytes` are encoded (head + tail layout).
-`examples/tx_legacy_demo`
Construct a `web3c_tx_legacy` transaction in memory, print its fields, and validate the struct (no RLP).
-`examples/tx_legacy_rlp_demo`
Encode a legacy transaction into its unsigned RLP preimage and print the resulting hex:
```bash
./examples/tx_legacy_rlp_demo
```
## Build

```bash
make
make test
make examples
```
This will build:
-`libweb3c.a` – the static library
-Test binaries under `tests/` and run them via `make test`
-Example binaries under `examples/` via `make examples`
You can also compile and run individual tests or examples manually, for example:
```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude \
    src/web3c_abi.c src/web3c_hex.c src/web3c_keccak.c src/web3c_tx.c src/web3c_rlp.c \
    tests/test_abi.c \
    -o test_abi_manual

./test_abi_manual
```
## Folder Structure

```text
.
├── docs/
│   └── architecture.md      # High-level design and module overview
├── examples/
│   ├── simple_encode.c
│   ├── abi_types_demo.c
│   ├── erc20_transfer_calldata.c
│   ├── setdata_bytes_calldata.c
│   └── tx_legacy_rlp_demo.c
├── include/
│   └── web3c/
│       ├── web3c.h          # Public umbrella header
│       ├── abi.h
│       ├── hex.h
│       ├── keccak.h
│       ├── tx.h
│       └── rlp.h
├── src/
│   ├── web3c_abi.c
│   ├── web3c_hex.c
│   ├── web3c_keccak.c
│   ├── web3c_tx.c
│   └── web3c_rlp.c
├── tests/
│   ├── test_abi.c
│   ├── test_keccak.c
│   ├── test_selector.c
│   ├── test_tx.c
│   └── test_rlp.c
├── Makefile
└── README.md
```
(If the actual layout differs slightly, prefer the real tree, but this is the intended structure.)
## Roadmap
### ABI
- Fixed-size arrays (e.g. uint256[2], bytes32[4])
- Multi-argument dynamic layouts (e.g. (string, bytes))
- Support for string via UTF-8 bytes

### Transactions
- Signed legacy transactions (RLP with v, r, s)
- EIP-1559-style transaction struct and encoder
- Helpers for transaction hashing (signing preimages)

### Crypto & Security
- Message hashing helpers (e.g. eth_sign prefix)
- Domain separation helpers for typed data (exploratory)

### Integration
- Optional JSON-RPC helpers (pluggable, no global state)
- Small CLI tools:
  - Offline calldata builder
  - Offline transaction builder

### Quality & Tooling
- Fuzz tests for ABI and RLP
- Continuous integration (CI) with multiple compilers
- Additional examples and cookbook-style documentation

## License
- Web3C is released under an Apache-2.0-style license with an additional attribution requirement.
- Free to use for personal and commercial projects
If used in a commercial product, a visible attribution must be included, such as:
"This product includes components from web3c (https://github.com/hesameworks/web3c) developed by Hesamedine."
See the LICENSE file for full details

## About
Web3C Library – a modular C library providing low-level, offline-friendly primitives for Ethereum/Web3: ABI encoding, transaction tools, and security-oriented helpers.