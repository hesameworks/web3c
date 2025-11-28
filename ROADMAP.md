# Web3C Roadmap

This document outlines potential directions for the Web3C project.  
Items are not promises, but a rough guide for future work.

---

## 1. ABI

- [ ] Support fixed-size arrays (e.g. `uint256[2]`, `bytes32[4]`)
- [ ] Support `string` as UTF-8 dynamic `bytes`
- [ ] Multi-argument dynamic layouts (e.g. `(string, bytes)`)
- [ ] Additional helpers for encoding common function patterns

---

## 2. Transactions

- [ ] Signed legacy transactions (`v`, `r`, `s` + full RLP)
- [ ] EIP-1559-style transaction struct and encoding
- [ ] Helpers for computing signing preimages
- [ ] Optional helpers for chain-specific defaults (e.g. mainnet, testnets)

---

## 3. RLP

- [ ] More generic list encoding helpers
- [ ] Additional primitive helpers as needed by higher-level modules
- [ ] Test vectors aligned with Ethereum reference implementations

---

## 4. Keccak & Crypto Helpers

- [ ] Message hashing helpers (e.g. `eth_sign` prefix)
- [ ] Basic building blocks for typed data / domain separation (exploratory)
- [ ] Integration tests combining Keccak + RLP + tx

---

## 5. Integration & Tooling

- [ ] Optional JSON-RPC helpers (no global state)
- [ ] CLI tools:
  - [ ] Offline calldata builder
  - [ ] Offline transaction builder (unsigned / signed)
- [ ] Example integration with other languages (Python/Rust bindings)

---

## 6. Quality & CI

- [ ] Continuous Integration via GitHub Actions
- [ ] Build against multiple compilers (gcc, clang)
- [ ] Fuzz testing for ABI and RLP
- [ ] Static analysis runs (clang-tidy, etc.)

---

## 7. Documentation

- [ ] Cookbook-style examples
- [ ] More diagrams in `docs/architecture.md`
- [ ] Security considerations section

---

If you are interested in working on any of these items, feel free to open an issue or start a discussion.