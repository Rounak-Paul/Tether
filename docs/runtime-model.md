# Tether — Runtime & Acceleration Model (Phase 1)

This document describes the runtime architecture choices for Phase 1, focusing on memory representation, device offload semantics, and APIs for controlling data movement.

## Runtime components

- Frontend: lexer → parser → typed AST.
- Type checker: enforces strong static typing and emits typed AST.
- Runtime core (C): interpreter loop, simple JIT/AOT hooks for kernels, device drivers.
- Device backends: CPU executor (thread pool), GPU backend(s) (CUDA/Vulkan/OpenCL prototypes), and a distributed executor (Phase 2).

## Memory & ownership model

- Managed heap for runtime objects (arrays, strings, structs). Memory is owned by `Array` objects unless explicitly converted to non-owning `Slice`.
- Borrowing model: `Slice[T]` borrows memory; runtime enforces lifetimes in the interpreter via reference counting or simple borrow checks at runtime. Phase 1 uses reference counting (RC) for simplicity.
- Explicit APIs for low-level control: `device_malloc`, `device_free`, `pin`, `prefetch` (host-visible controls for performance-critical code).

## Data movement & device placement

- Heuristics: the runtime will automatically offload large numeric kernels/array operations to GPU when:
  - A compatible backend is available.
  - The operation is data-parallel and memory-bound (large arrays).
  - The user did not opt-out via attributes.

- Explicit hints: `@gpu` decorator or `device_hint(arr, GPU)` to force placement.
- Data semantics: copies are copy-on-demand. When a kernel requires device-local data, the runtime copies the required buffers and records ownership. On return, results are synced back unless the memory is pinned on the device.

## Kernel compilation & fallback

- Kernels annotated `@kernel` are compiled to simple device code via a small codegen path. For Phase 1 this may be a naive code emitter or a stub that maps to CPU loops.
- If device compilation/offload fails at runtime, the runtime falls back to a CPU implementation and records an informative warning and metrics.

## Threading & parallel execution

- CPU executor: fixed-size thread pool with work-stealing for `parallel for` and internal parallel ops.
- `parallel for` maps to per-chunk tasks; the runtime ensures no shared-write races unless the user uses explicit atomic operations or structured synchronization primitives.

## Diagnostics & profiling

- The runtime should expose simple profiling APIs and counters: kernel time, data transfer bytes, fallback counts.
- Provide human-friendly diagnostics for offload decisions (why a kernel was or wasn't offloaded).

## Minimal runtime API sketches

- device_malloc(size, device) -> DevicePtr
- device_free(DevicePtr)
- pin(arr: Array[T], device)  // pin array to device memory
- prefetch(arr: Array[T], device)  // hint to copy
- device_hint(fn, device)  // request placement for a function

## Next steps (Phase 1)

- Implement the typed AST and type-checker.
- Wire a simple interpreter loop that understands typed variable declarations and function calls.
- Implement a CPU-based lowering for `@kernel` functions (so tests can run without GPUs), then add a GPU backend prototype.
