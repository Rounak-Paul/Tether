# Tether Programming Language

Tether is a new programming language designed for simplicity and performance, empowering users to write code that is automatically accelerated across CPUs, GPUs, or distributed nodes—without explicit device management. Write your logic once, and Tether ensures it runs fast everywhere.

---

## 🚀 Project Vision

- **Simple, expressive syntax:** Aims to be as easy as Python, as powerful as C++/Rust.
- **Transparent acceleration:** Automatically leverages available CPU, GPU, or cluster resources.
- **Zero-boilerplate parallelism:** Native constructs for parallelism, distribution, and data movement.
- **Safe by default:** Eliminates common pitfalls of manual threading, memory management, and device APIs.

---

## Developer TODO (technical roadmap)

Below is a detailed, checkbox-style technical roadmap you can work through one item at a time. Each top-level task lists concrete substeps, the primary files to touch, acceptance criteria, and suggested next steps. Use these checkboxes to track progress in the repository and in issue/PR descriptions.

### 1) Parser & AST
- [ ] 1.1 Add parser golden tests
	- Files: `tests/parser/`, `parser-runner`, `src/parser.c`
	- Substeps:
		- [ ] Create representative `.tx` inputs (control flow, precedence, functions, calls).
		- [ ] Add golden output files under `tests/parser/golden/`.
		- [ ] Implement a test harness or Make target to compare parser output to golden files.
	- Acceptance: `make test-parser` or `parser-runner --test tests/parser/*.tx` exits 0 and prints diffs on failure.

- [ ] 1.2 Implement an AST pretty-printer
	- Files: `include/ast.h`, `src/ast.c`, `parser-runner`
	- Substeps:
		- [ ] Define human-readable node formatting in `ast.h` comments.
		- [ ] Implement `ast_pretty_print()` that emits an indented tree.
		- [ ] Wire `parser-runner --ast` to call the pretty-printer instead of raw dumps.
	- Acceptance: `./parser-runner examples/hello.tx --ast` prints a clean tree.

- [ ] 1.3 Validate AST shapes and invariants
	- Files: `include/ast.h`, `src/ast.c`, `src/parser.c`
	- Substeps:
		- [ ] Add unit tests asserting node children counts and node kinds for representative inputs.
		- [ ] Document AST node shapes in `docs/ast.md` (brief summary now, fuller later).
	- Acceptance: unit tests verifying AST node shapes pass.

### 2) Tokenizer & Lexer
- [ ] 2.1 Harden tokenization for edge cases
	- Files: `src/token.c`, `include/token.h`, `src/lexer.c`
	- Substeps:
		- [ ] Enumerate all token kinds and document them in comments.
		- [ ] Add tests for multi-char tokens (e.g., `->`, `==`, `!=`, `<=`, `>=`).
		- [ ] Add tests for comments, escaped strings, and unterminated tokens.
	- Acceptance: tokenizer unit tests pass reliably.

- [ ] 2.2 Normalize token output
	- Files: `src/token.c`, `src/lexer.c`
	- Substeps:
		- [ ] Ensure token structs include kind, lexeme, and source position.
		- [ ] Add equality helpers to make test assertions easy.
	- Acceptance: tests use equality helpers and are simpler to write and read.

### 3) Interpreter: scoping, loops, builtins
- [ ] 3.1 Implement lexical scopes and environments
	- Files: `src/interpreter.c`, `src/interpreter.h`, `include/ast.h`
	- Substeps:
		- [ ] Replace global-local hack with an Environment struct (parent pointer + map).
		- [ ] Implement variable lookup with shadowing rules.
		- [ ] Add unit tests for nested blocks and shadowing.
	- Acceptance: scope-related tests pass and behavior matches documented examples.

- [ ] 3.2 Implement loop semantics and control flow
	- Files: `src/interpreter.c`, `src/tx_runner.c`
	- Substeps:
		- [ ] Implement `while` and `for` AST evaluation paths.
		- [ ] Implement `break` and `continue` handling (control flow signals/exceptions).
		- [ ] Add tests in `tests/` exercising loops and early exits.
	- Acceptance: loop examples in `examples/` print expected stdout and exit status.

- [ ] 3.3 Add core builtins
	- Files: `src/runtime.c`, `src/interpreter.c`, `include/runtime_api.h`
	- Substeps:
		- [ ] Implement `print`, `len`, `assert` with documented behavior.
		- [ ] Wire builtins into the global environment at program start.
		- [ ] Add tests for builtin behavior and error messages.
	- Acceptance: examples and tests using builtins pass.

### 4) Typechecker stabilization
- [ ] 4.1 Expand type tests
	- Files: `src/typechecker.c`, `tests/type/`
	- Substeps:
		- [ ] Add tests for call arg mismatches, return mismatches, shadowing, and block scopes.
		- [ ] Stabilize error messages (format and content) for deterministic test outputs.
	- Acceptance: `tests/type/` passes locally.

- [ ] 4.2 Introduce a `Type` representation
	- Files: `include/ast.h`, `src/typechecker.c`
	- Substeps:
		- [ ] Define a `Type` struct or enum that encodes primitive and composite kinds.
		- [ ] Update typechecker to use `Type` values instead of raw strings.
		- [ ] Add unit tests that assert expected `Type` results.
	- Acceptance: typechecker tests pass; code is clearer and easier to extend.

### 5) Tests, Make targets, and CI
- [ ] 5.1 Add `make test` and granular targets
	- Files: `Makefile`, helper scripts
	- Substeps:
		- [ ] Add `make test-parser`, `make test-type`, `make test-interp`, and aggregate `make test`.
		- [ ] Ensure `make` builds artifacts required by tests.
	- Acceptance: running `make test` on a clean checkout builds and runs tests.

- [ ] 5.2 Add GitHub Actions workflow
	- Files: `.github/workflows/ci.yml`
	- Substeps:
		- [ ] Create a simple workflow that checks out the repo, installs toolchain, runs `make`, and `make test`.
		- [ ] Cache build artifacts where appropriate to speed up runs.
	- Acceptance: PRs trigger CI and the workflow runs on Ubuntu and macOS runners.

### 6) Runners & UX improvements
- [ ] 6.1 Add helpful runner flags
	- Files: `parser-runner`, `tx-runner`, `src/parser_runner.c`, `src/tx_runner.c`, `scripts/run_tx.sh`
	- Substeps:
		- [ ] Add `--ast`, `--verbose`, and `--repl` flags with clear help text.
		- [ ] Add small unit tests or smoke tests for flag parsing and behavior.
	- Acceptance: runners accept flags and behave as documented.

- [ ] 6.2 Improve `scripts/run_tx.sh`
	- Files: `scripts/run_tx.sh`
	- Substeps:
		- [ ] Add options to run parser, typechecker, or interpreter.
		- [ ] Add a `--watch` or `--repeat` helper for quick iterations (optional).
	- Acceptance: script is a convenient entrypoint for iterating on `.tx` files.

### 7) Developer documentation
- [ ] 7.1 Create `docs/ast.md` and `docs/developer.md`
	- Files: `docs/ast.md`, `docs/developer.md`, `README.md`
	- Substeps:
		- [ ] Document AST node shapes, token kinds, and interpreter value model.
		- [ ] Add a short contributor guide: how to add tests, implement a new AST node, and run CI.
	- Acceptance: a new contributor can follow docs to add a parser case and corresponding interpreter support.

### 8) Examples & golden inputs
- [ ] 8.1 Add more example programs
	- Files: `examples/`, `tests/parser/golden/`, `tests/interp/expected/`
	- Substeps:
		- [ ] Add deterministic examples that exercise language features (loops, functions, scope, I/O).
		- [ ] Capture expected outputs and add them as golden files for tests.
	- Acceptance: examples double as tests and help prevent regressions.

---

Notes and suggested workflow
- Work top-to-bottom for the fastest feedback loop: parser -> lexer -> interpreter -> typechecker -> CI.
- Keep changes small and add tests alongside implementation.
- When working on a todo, update this README checkbox and open a PR with a focused title (e.g., "parser: add AST pretty-printer and golden tests").

If you'd like, I can:
- Start with item 1.2 (AST pretty-printer) and create a minimal implementation and tests.
- Or generate the `Makefile` test targets and a basic GitHub Actions workflow.

Next step: pick which item to work on first and I will mark it in the project's todo list and start implementing.


## Quickstart — write and run a .tx file

Create a file such as `examples/hello.tx` and then run the parser and typechecker on it using the helper script:

```
./scripts/run_tx.sh examples/hello.tx
```

The script prints a short parse summary followed by typecheck output. This is a lightweight way to start writing and iterating on `.tx` programs.

Next steps: add more examples under `examples/`, or extend `scripts/run_tx.sh` to run a REPL or interpreter as those components are added.
