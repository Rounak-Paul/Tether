# Tether — Core Requirements

This document records the outcomes of Phase 0 work: decisions about the language's core goals, primary users, syntax principles, essential features, runtime model, and a short "contract" and testable acceptance criteria. It's intentionally concise and actionable so it can guide Phase 1 implementation.

## Purpose

Tether aims to be a small, expressive systems-to-numerics language that:

- Lets users write straightforward code that runs efficiently on CPUs, GPUs, or distributed nodes without manual device management.
- Provides safe-by-default abstractions for parallelism and distributed execution.
- Scales from single-file scripts to distributed pipelines with minimal changes to user code.

## Primary Goals (high level)

1. Simplicity: syntax and ergonomics friendly to data scientists and domain engineers.
2. Performance: efficient execution on modern hardware (multicore CPU, discrete GPUs, clusters).
3. Transparency: automatic device placement and data movement, with explicit opt-in for manual control.
4. Safety: minimize race conditions, memory bugs, and surprising semantics.
5. Interoperability: FFI for C and easy integration with host tooling.

## Target Users & Personas

- Data scientists / ML researchers: want fast array kernels without writing CUDA/OpenCL.
- HPC / simulation engineers: want deterministic parallel constructs and low-level control when needed.
- Systems engineers / backend devs: want to build fast pipelines that transparently scale to multiple nodes.
- Language/tooling contributors: language should be easy to implement and test (initially in C).

## Success Criteria (for Phase 0 → Phase 1 handoff)

- A concise spec exists describing syntax, primitives, and runtime expectations. (this file)
- Minimal examples for: scalar code, parallel loop, and a GPU-accelerated array op.
- A clear contract (inputs/outputs/behavior) for the runtime and acceleration model.

## Contract (inputs, outputs, error modes, guarantees)

- Inputs: Tether source files or interactive REPL inputs.
- Outputs: deterministic program results (unless using explicitly non-deterministic constructs), side-effecting I/O, and performance metrics.
- Error modes: syntax errors (compile/parse), static type/annotation errors (optional), runtime errors (exceptions), and device-offload failures.
- Guarantees:
  - Correctness: semantics of sequential code match the single-threaded reference.
  - Race-freedom by default for shared-memory parallel constructs unless user explicitly uses unsafe primitives.
  - Memory safety within the runtime's managed types; FFI and manual device buffers are opt-in and unsafe.

## Syntax Design Principles

-- Human-friendly: favor readable, low-ceremony syntax while keeping explicit, strongly-typed declarations.
-- Explicit when performance matters: mandatory type annotations and explicit kernel decorators for GPU code.
- Small core: a few orthogonal primitives (functions, modules, arrays, parallel_for, decorators/attributes).
- Consistency: similar constructs behave similarly across CPU/GPU/distributed execution.


Design choices for Phase 1 (bootstrap in C):

-- Use semicolons to terminate statements and braces (`{ ... }`) for block scope (C/Java/C++ style). This keeps parsing unambiguous and matches many systems programmers' expectations.
- Use a C-like expression syntax for arithmetic and indexing (familiar to systems engineers).
-- Require explicit type annotations using the compact syntax: `x: i32 = 3;` (types are mandatory for top-level/local variables, function params, and returns).
- Keep decorators/attributes for acceleration hints: `@gpu`, `@kernel`, `@device(local=true)` — decorators appear immediately before the declaration they modify.

### Example snippets

Scalar function (C-like blocks, semicolons):

```
def add(a, b) {
  return a + b;
}

res = add(1, 2);
print(res);
```

Parallel loop (shared-memory parallel_for, C-style):

```
arr = zeros(1024);

parallel for (i = 0; i < len(arr); i++) {
  arr[i] = heavy_compute(i);
}
```

GPU kernel (explicit kernel decorator; runtime may auto-offload some annotated functions):

```
@kernel
def saxpy(a: f32, x: Array[f32], y: Array[f32]) {
  i = global_idx();
  if (i < len(x)) {
    y[i] = a * x[i] + y[i];
  }
}

// host call
saxpy(2.0, x, y);
```

## Core Features (minimal for MVL)

- Primitive types: integers (i8/i16/i32/i64), unsigneds, floats (f32/f64), bool, string.
- Composite types: fixed-size and dynamic Arrays, Tuples, lightweight Structs.
- Functions and first-class closures (initially limited closures in kernels).
- Modules/imports and a small standard library (math, random, IO, array ops).
- Parallel primitives: `parallel for`, `map`, `reduce`, thread-safe channels (later).
- Acceleration primitives: `@kernel`, `@gpu`, automatic device placement for array ops.
- Memory model: managed heap for runtime objects; views/borrowed slices for zero-copy operations.
- FFI: simple C header-driven interop for numeric kernels.

## Type System (Phase 1 decision)

- Strong static typing (mandatory annotations): Tether will be strongly and statically typed. Function parameters, return types, and top-level/local variable declarations must include explicit type annotations. This choice maximizes reliability and early error detection.

- No implicit widening or narrowing conversions between numeric types. Conversions must be explicit via cast operators (e.g., `f32(x)` or `i32(x)`).

- Generics for containers: `Array[T]`, `Tuple[T1, T2]`, etc., with monomorphization or specialization in the runtime/compiler.

- Kernel and parallel entry points: must have fully specified, non-polymorphic parameter types (element types for arrays, scalar types for numerics). This ensures safe and predictable device code generation.

- Local type inference (optional and limited): the compiler/runtime may infer types for obvious literals and local expressions, but inference is conservative — any ambiguous or polymorphic use must be annotated.

- Error behavior: type errors are compile-time (or parse-time for the interpreter bootstrap). The runtime will refuse to execute code with unresolved type errors.

Example annotations:

```
def add(a: i32, b: i32) -> i32 {
  return a + b;
}

arr: Array[i32] = zeros(1024);
```

## Runtime & Acceleration Model

- Single runtime (C) that performs:
  - Parsing & AST building.
  - Lightweight JIT/AOT decision: initially interpret AST and compile hot kernels to simple device code where feasible.
  - Automatic device placement heuristics: promote large array ops to GPU when available and profitable.
  - Data movement: transparent copy-on-demand, with explicit APIs to prefetch or pin memory.

- Execution fallback: if an offload fails, runtime falls back to CPU implementation and returns an informative error.

## Edge Cases & Known Tradeoffs

- Indentation-based syntax simplifies parsing but requires careful tooling (formatters, editors).
- Automatic offload simplifies UX but can obscure performance costs; provide profiling and explicit hints.
- Memory model choice: managed runtime simplifies safety but may limit peak performance; provide escape hatches.

## Minimal Acceptance Tests (Phase 1)

1. Parser test: parse the scalar and parallel examples above without errors.
2. Interpreter test: run `add(1,2)` and `parallel for` on CPU producing correct results.
3. Kernel test: annotate a simple SAXPY kernel, run on GPU (if available), and fall back to CPU when not.

## Next steps (short)

- Implement a `docs/` example folder with three example scripts (scalar, parallel_for, kernel).
- Draft a minimal grammar and start on the lexer and parser in C (Phase 1).
- Add tests corresponding to the minimal acceptance tests.

---

Document maintained as the Phase 0 concise specification for handoff into development. Update iteratively as decisions change.
