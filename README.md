# Web3C

Web3C is a lightweight C library that provides low-level, offline-friendly primitives for interacting with Ethereum-like (EVM-based) blockchains.

> Status: **Experimental** – API is not stable yet.

---

## ✨ Features

### ABI Encoding

Minimal ABI encoder for core Solidity types:

- `uint256` (from `uint64_t`)
- `address` (20-byte)
- `bool`
- `bytes32`
- `bytes` (dynamic, tail encoder)

Supported patterns:

- Single value encoding (e.g. `uint256` → 32-byte word)
- Static head arguments
- Dynamic `bytes` tail (offset + length + data + padding)

### Function Selectors

- Compute 4-byte function selectors from Solidity signatures:
  - `"transfer(address,uint256)"` → `a9059cbb`
- Uses Ethereum-style Keccak-256 under the hood.

### Hex Utilities

- Binary → hex (lowercase)
- Hex → binary
- Useful for printing ABI, RLP and hash outputs in CLI tools.

### Keccak-256

Ethereum-style Keccak-256 hash function:

- One-shot API
- Streaming API

### Legacy Transaction Primitives

Minimal representation of Ethereum legacy transactions:

- `web3c_tx_legacy` struct:
  - `nonce`
  - `gas_price`
  - `gas_limit`
  - `value`
  - `chain_id`
  - `has_to` + `to[20]`
  - `data` pointer + `data_len`

Helpers for:

- Initialization
- Setting fields
- Basic validation

### RLP Encoding

Minimal RLP primitives:

- `uint64` → RLP string
- `bytes` → RLP string
- List header

Legacy transaction RLP (unsigned EIP-155 preimage):

```text
[ nonce,
  gasPrice,
  gasLimit,
  to,
  value,
  data,
  chainId,
  0,
  0 ]
```
Suitable as preimage for:
- Keccak-256 hashing
- secp256k1 signing (handled externally)

## 🎯 Who is this for?
Web3C targets developers who:
- Work in C and want to interact with Ethereum-like chains.
- Need offline / air-gapped tooling (calldata builders, tx preimage tools, etc.).
- Care about small, auditable C code instead of large SDKs.
- Build security / research utilities around EVM internals.
It is not a full Web3 SDK. It is a focused, low-level toolkit.

## 📁 Project Structure
```text
.
├── docs/
│   └── architecture.md      # High-level design and module overview
├── examples/
│   ├── simple_encode.c
│   ├── abi_types_demo.c
│   ├── erc20_transfer_calldata.c
│   ├── setdata_bytes_calldata.c
│   ├── tx_legacy_demo.c
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
├── ATTNIBUTION
├── LICENSE
└── README.md
```
## 🧪 Examples
All examples are built with:
```bash
make examples
```
Then you can run:
| Example                            | Description                                                     |
| ---------------------------------- | --------------------------------------------------------------- |
| `examples/simple_encode`           | Encode a `uint256` into a 32-byte ABI word and print as hex.    |
| `examples/abi_types_demo`          | Showcase of `uint256`, `bool`, `bytes32`, and dynamic `bytes`.  |
| `examples/erc20_transfer_calldata` | Build ERC-20 `transfer(address,uint256)` calldata from CLI.     |
| `examples/setdata_bytes_calldata`  | Build calldata for `setData(bytes)` with dynamic bytes.         |
| `examples/tx_legacy_demo`          | Construct a `web3c_tx_legacy` in memory and print its fields.   |
| `examples/tx_legacy_rlp_demo`      | Encode a legacy tx into an unsigned RLP preimage and print hex. |

### Example:
```bash
./examples/erc20_transfer_calldata 0x00112233445566778899aabbccddeeff00112233 1000
./examples/tx_legacy_rlp_demo
```
## 🛠 Build & Test
Build the static library:
```bash
make
```
Run the test suite:
```bash
make test
```
Build examples:
```bash
make examples
```
You can also compile a single test manually:
```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude \
    src/web3c_abi.c src/web3c_hex.c src/web3c_keccak.c src/web3c_tx.c src/web3c_rlp.c \
    tests/test_abi.c \
    -o test_abi_manual

./test_abi_manual
```
## 🧱 Design Philosophy

- Offline-first – works without network or JSON-RPC.
- No dynamic allocation in core modules – caller provides all buffers.
- Small, explicit API – easier to audit and reason about.
- Bindings-friendly – can be wrapped from Python, Rust, Go, etc.
- No global state – safe for multi-threaded / embedded contexts.

See [docs/architecture.md](https://github.com/hesameworks/web3c/blob/main/docs/architecture.md) for more details.

## 🗺 Roadmap (high-level)
See [ROADMAP.md](https://github.com/hesameworks/web3c/blob/main/ROADMAP.md) for a more detailed list.

Planned areas include:
- More ABI types (arrays, string)
- Signed legacy transactions
- EIP-1559-style transactions
- Optional JSON-RPC helpers
- CLI tools (offline calldata / tx builder)
- Fuzz testing & CI

## 🤝 Contributing

Contributions are welcome — from small bug fixes and tests to new examples and documentation improvements.
See [CONTRIBUTING.md](https://github.com/hesameworks/web3c/blob/main/CONTRIBUTING.md) for details

## ⚖️ License
Web3C is released under an Apache-2.0-style license with an additional attribution requirement.

Free for personal and commercial use.

If used in a commercial product, a visible attribution must be included, such as:

"This product includes components from web3c [hesameworks](https://github.com/hesameworks/web3c)developed by Hesamedine."

See [LICENSE](https://github.com/hesameworks/web3c/blob/main/LICENSE) for the full text.