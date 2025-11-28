# Contributing to Web3C

Thanks for your interest in improving **Web3C**!  
This project is experimental, but contributions of all sizes are welcome — from documentation fixes to new tests, features, or examples.

---

## 🧰 Build Instructions

To build the library, tests, and examples:

```bash
make
make test
make examples
```

This will:
- Build the static library (libweb3c.a)
- Build and run all tests under tests/
- Build all example programs under examples/

## 📐 Code Style

To keep the codebase consistent and readable:
- Use C11 (-std=c11)
- Enable warnings: -Wall -Wextra -Wpedantic
- No dynamic allocation in core modules (abi, keccak, tx, rlp)
- Prefer small, focused functions
- Keep headers organized under include/web3c/
- Comments and public APIs must be in clear English
If you are unsure about naming or structure, feel free to ask in an issue before coding.

## 🧪 Tests

If you modify behavior or add a new feature:
- Add/update tests under tests/
- Ensure make test passes before submitting a PR
- Keep tests small and isolated
Even documentation-only or example-only PRs are welcome.

## 📄 Examples

If you add new features, consider adding a small example under examples/.

Guidelines:
- Keep examples self-contained
- Include argument parsing if needed
- Demonstrate real usage (ABI, calldata, tx, RLP, hashing, etc.)
- Prefer clarity over cleverness

## 🔀 Pull Request Guidelines

- Fork the repository
- Create a feature branch
- Keep commits clean and descriptive
    - Example: feat: add EIP-1559 tx struct
    - Example: fix: handle zero-length bytes in RLP
- Open a PR against main
- In the PR description, include:
    - What you changed
    - Why it matters
    - Any notes for reviewers
All contributions will be reviewed as soon as possible.

## 💬 Discussions & Issues

If you want to propose:
- A new module
- A feature
- A test scenario
- A research direction
- Documentation improvements

Please open an Issue or a Discussion first.
This keeps the project focused and prevents duplicated effort.

## 🙏 Thank You
Thank you for helping make Web3C better.
Every contribution — small or large — helps move the project and the ecosystem forward.