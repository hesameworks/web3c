# Web3C

**Web3C** is a lightweight C library providing low-level, offline-friendly primitives for interacting with Web3 / Ethereum-like blockchains.

> Status: **Experimental – API is not stable yet.**

## Features (Phase 1)

- Minimal ABI encoding:
  - `uint256` (from `uint64_t`)
  - `address` (20-byte)
- Hex utilities:
  - Binary to hex (lowercase)
  - Hex to binary

## Roadmap

- Keccak-256 implementation and function selector helpers.
- Full static ABI encoding for:
  - `bool`, `bytes32`, arrays
- Transaction building primitives for EVM-compatible chains.
- JSON-RPC client helpers (optional, pluggable).

## Build

```bash
make
make test
```

## License
- TBD (MIT/BSD-style is recommended).