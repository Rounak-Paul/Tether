# Tether — Type Checker Design (Phase 1)

This document outlines the type-checker pass: goals, algorithm, diagnostics, and a small set of unit-style test cases. It's intended to guide the Phase 1 C implementation.

## Goals

- Enforce the strong static typing rules from `docs/type-system.md`.
- Produce clear, actionable diagnostics with source locations and suggestions.
- Be fast and conservative; prefer clear error messages over over-eager inference.

## Input & output

- Input: typed AST (parser attaches syntactic nodes; type annotations are present on params/decls).
- Output: typed AST with resolved/mapped concrete types, or a list of type errors (no code generation if errors exist).

## Main checks

1. Declaration checks:
   - Variables must have an explicit type in the declaration. Verify that the initializer expression's type is equal to declared type or that an explicit cast is present.
2. Expression checks:
   - Binary ops: both operands must have identical types for arithmetic/comparison (except explicit cast nodes).
   - Function calls: argument count must match, and each arg's type must equal the parameter type.
3. Control flow:
   - `if` conditions must be `bool`.
   - Loop indices and control expressions must be properly typed per grammar (e.g., `for` init/cond/post expression types must be valid).
4. Kernel restrictions:
   - Kernel parameters must be concrete (no generics, no function pointers).
   - Kernel-local variables must have concrete types from the allowed kernel set.

## Algorithm sketch

- Walk AST in a single pass with the following phases:
  1. Symbol collection: collect function signatures, global variables, and types into symbol table.
  2. Body checking: for each function, type-check body with a local symbol scope. Resolve expression types bottom-up.
  3. Post-check: validate kernel constraints and cross-module references.

## Diagnostics

- Error format (example):

  file.tether:23:5: TypeError: expected `i32` but found `f32` in assignment to `x`
  -> Suggestion: use `i32(expr)` to convert explicitly.

- Use specific error codes for programmability: TYPE_MISMATCH, UNDECLARED_SYMBOL, ARITY_MISMATCH, KERNEL_INVALID_TYPE.

## Example test cases (in `tests/type/`)

- `good_add.tether` — a minimal, valid typed example.
- `bad_mismatch.tether` — assignment with mismatched types (expect TYPE_MISMATCH).
- `kernel_bad.tether` — kernel with polymorphic or missing types (expect KERNEL_INVALID_TYPE).

## Next steps for implementers

- Implement a small symbol table (scoped) and typed AST nodes.
- Implement the checks above and wire tests to fail/pass based on returned error lists.
- Add pretty-printing for diagnostics and a small runner that prints PASS/FAIL for test snippets.
