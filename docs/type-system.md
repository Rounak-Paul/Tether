# Tether — Type System (Phase 1)

This document expands the strong static typing decisions for Tether and provides concrete rules, runtime mapping, and examples. The goal is to make type behavior precise for the Phase 1 C bootstrap and early compiler/type-checker work.

## High-level goals

- Strong, predictable static typing for reliability and early error detection.
- Minimal implicit behavior: no implicit numeric conversions or surprising promotions.
- Clear, monomorphized types for kernels and device code.

## Primitive types

- Signed integers: i8, i16, i32, i64
- Unsigned integers: u8, u16, u32, u64
- Floats: f32, f64
- Bool
- String (immutable, UTF-8)

Each primitive maps to a straightforward C primitive during bootstrap (e.g., `i32` → `int32_t`, `f32` → `float`).

## Composite & container types

- Array[T]: a contiguous, dynamically-sized array of elements of type T. Arrays are typed; `Array[i32]` is different from `Array[f32]`.
- Slice/Views: a non-owning view `Slice[T]` or `&[T]` that references existing array data without owning it.
- Tuple[T1, T2, ...]
- Structs: lightweight aggregate types with named fields; field types must be concrete.

## Functions & generics

- Functions must declare parameter and return types: `def foo(a: i32, b: f32) -> f32 { ... }`.
- Generic functions are allowed but must be instantiated before device/kernel use. For Phase 1, prefer explicit monomorphization: `def map[T](arr: Array[T], f: Fn[T]->T) -> Array[T]` can be specialized for concrete Ts before kernel compile.

## Type rules & checking

- No implicit numeric conversions. Mixing numeric types requires explicit casts: `f32(x)` or `i32(y)`.
- Binary arithmetic requires operands of the same type (or explicitly cast). The result has that same type.
- Assignments require exact type match: the expression's type must equal the variable's declared type (modulo explicit casts).
- Function overloading is not supported in Phase 1 (name + signature uniqueness enforced).

### Casts

- Explicit cast syntax: `TargetType(expr)` e.g., `f32(1)` or `i32(x)`.
- Casting semantics:
  - Narrowing casts (e.g., `i32` → `i16`) truncate or wrap according to two's complement semantics for integers (documented clearly), but are explicit and flagged by the type checker as potentially lossy.
  - Float→int casts truncate toward zero unless a library cast with rounding is used.

## Literals & type inference

- Literals have default types but must be annotated where required:
  - Integer literals default to `i64` unless suffixed or contextually typed.
  - Floating literals default to `f64` unless suffixed or contextually typed.
- Local type inference is conservative: the compiler can infer local variable types from a single initializer when unambiguous, but top-level and function-local declarations must include explicit type annotations in Phase 1.

## Kernel & parallel restrictions

- Kernel entry points must have fully concrete parameter types (no generic parameters) and element-level types for arrays.
- Kernels cannot accept polymorphic function pointers or closures in Phase 1.
- Kernels may only use a limited set of intrinsics and types (primitive scalars, typed arrays, fixed-size structs). This restriction simplifies backend code generation.

## Runtime type representation

- Primitive types map to fixed-size C types (`i32`→`int32_t`, etc.).
- `Array[T]` runtime representation:
  - struct {
      void *data;        // pointer to element storage
      usize len;         // number of elements
      usize capacity;    // allocated capacity in elements
      TypeTag elem_tag;  // runtime tag or descriptor for the element type
    }

- `Slice[T]` is a lightweight pointer/len pair referencing an `Array[T]` or some other buffer; it does not own memory.

## Errors and diagnostics

- All type mismatches are reported at compile-time. The type checker will list the expression location, expected type, and actual type, plus a suggested fix when possible (e.g., suggest a cast).

## Examples

```
def add(a: i32, b: i32) -> i32 {
    return a + b;
}

arr: Array[i32] = zeros(1024);

// explicit cast
f: f32 = f32(1);
```

## Next steps for Phase 1 implementation

- Implement a typed AST that carries concrete types for expressions and declarations.
- Implement a type-checker pass that enforces these rules and emits clear diagnostics.
- Add unit-tests covering cast behavior, literal typing, kernel type restrictions, and container typing.
