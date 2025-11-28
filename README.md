# Web3C

**Web3C** is a lightweight C library providing low-level, offline-friendly primitives for interacting with Web3 / Ethereum-like blockchains.

> Status: **Experimental – API is not stable yet.**

## Features (Phase 1)

- Minimal ABI encoding for core Solidity types:
  - `uint256` (from `uint64_t`)
  - `address` (20-byte)
  - `bool`
  - `bytes32`
  - `bytes` (dynamic, single-argument tail encoder)
- Function selector helper:
  - Compute 4-byte selectors from Solidity signatures
    (e.g. `"transfer(address,uint256)"` → `a9059cbb`).
- Hex utilities:
  - Binary → hex (lowercase)
  - Hex → binary
- Ethereum-style **Keccak-256**:
  - One-shot and streaming API.
- Legacy Ethereum transaction primitives:
  - `web3c_tx_legacy` struct (nonce, gas, value, to, chain_id, data)
  - Small helpers for init, setting fields, and basic validation.

## Motivation

Most Web3 tooling is written in high-level languages (TypeScript, Python, Rust).
Web3C targets a different layer:

- Embedded systems.
- Minimal command-line tools.
- Security and research tooling.
- Environments where a small, auditable C library is preferred over a large SDK.

The goal is not to replace existing Web3 SDKs, but to provide a **thin, predictable
layer** that can be embedded almost anywhere.

## Design Goals

- **Offline-first** – core features work without any network or JSON-RPC.
- **No dynamic allocation** in core modules – the caller controls all memory.
- **Small and explicit API** – easy to audit and reason about.
- **Friendly to FFI and bindings** – simple C types, no global state.

## Examples

Some small examples live under the `examples/` directory:

- `examples/simple_encode`  
  Encode a `uint256` into a 32-byte ABI word and print it as hex.

- `examples/abi_types_demo`  
  Demonstrate ABI encoding for `uint256`, `bool` and `bytes32`.

- `examples/erc20_transfer_calldata`  
  Build ERC-20 `transfer(address,uint256)` calldata from CLI arguments.

- `examples/setdata_bytes_calldata`  
  Build calldata for a hypothetical `setData(bytes)` function,
  showing how dynamic `bytes` are encoded (head + tail layout).


## Roadmap

- ABI:
  - Fixed-size arrays (e.g. `uint256[2]`)
  - More dynamic types (e.g. `string`, multi-argument layouts)
- Transactions:
  - RLP size estimation and encoding for `web3c_tx_legacy`
  - Support for EIP-1559 style transactions (maxFee, maxPriorityFee)
- Crypto & security:
  - Small helpers around message hashing and domain separation
- Integration:
  - Optional JSON-RPC helpers (pluggable, no global state)
  - Small CLI tools built on top of Web3C (offline calldata / tx builder)

## Build

```bash
make
make test
```
This will build:
- libweb3c.a – the static library.
- Test binaries under tests/ and run them via make test.

## License
- TBD (MIT/BSD-style is recommended).