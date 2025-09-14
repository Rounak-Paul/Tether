# Tether Programming Language

Tether is a new programming language designed for simplicity and performance, empowering users to write code that is automatically accelerated across CPUs, GPUs, or distributed nodes—without explicit device management. Write your logic once, and Tether ensures it runs fast everywhere.

---

## 🚀 Project Vision

- **Simple, expressive syntax:** Aims to be as easy as Python, as powerful as C++/Rust.
- **Transparent acceleration:** Automatically leverages available CPU, GPU, or cluster resources.
- **Zero-boilerplate parallelism:** Native constructs for parallelism, distribution, and data movement.
- **Safe by default:** Eliminates common pitfalls of manual threading, memory management, and device APIs.

---

## 📋 Development TODO & Checkpoints

### Phase 0: Planning & Research

- [x] Study core requirements for the language (syntax, features, target users)
- [ ] Research existing solutions (Julia, Taichi, Chapel, Futhark, Ray, etc.)
- [ ] Draft the language’s unique selling points and initial README (this file!)
- [ ] Define a minimum viable language (MVL) spec (see Phase 1)

---

### Phase 1: Bootstrapping (in C)

#### 1. **Core Infrastructure**
- [ ] Set up project structure, build system, and CI
- [ ] Write code style guidelines and contribution docs
- [ ] Define coding standards for C core

#### 2. **Lexer & Parser**
- [ ] Design basic syntax (variables, functions, control flow)
- [ ] Implement a lexer (tokenizer) in C
- [ ] Implement a parser to build an AST (Abstract Syntax Tree)
- [ ] Write tests for lexer and parser

#### 3. **Interpreter/VM**
- [ ] Design a minimal runtime (memory, value types, error handling)
- [ ] Implement an AST interpreter (eval loop)
- [ ] Support variables, math, conditionals, functions, basic I/O
- [ ] Write a REPL (Read-Eval-Print Loop)
- [ ] Add basic error messages and debugging output

#### 4. **Basic Standard Library**
- [ ] Implement core types (numbers, strings, arrays, maps)
- [ ] Add basic stdlib functions (print, file I/O, math)
- [ ] Write sample scripts and test cases

---

### Phase 2: Parallelism & Acceleration

#### 1. **CPU Parallelism**
- [ ] Add language constructs for parallel loops (e.g., `parallel for`)
- [ ] Implement thread pool or task scheduling in C runtime
- [ ] Auto-detect parallelizable code blocks in the interpreter

#### 2. **GPU Support**
- [ ] Research and design GPU offload abstraction (Vulkan/CUDA/OpenCL backend)
- [ ] Implement basic GPU kernel execution for simple array ops
- [ ] Write language syntax for marking GPU-accelerated blocks
- [ ] Abstract data movement between CPU & GPU

#### 3. **Distributed Execution**
- [ ] Design primitives for distributed data and computation
- [ ] Implement node discovery & messaging (local network or cloud)
- [ ] Support distributed map/reduce and dataflow constructs

---

### Phase 3: Language Growth

#### 1. **Advanced Features**
- [ ] Add user-defined types, modules/imports, FFI (C interop)
- [ ] Improve error handling, type inference, and debugging tools
- [ ] Add support for hot-reloading code and live editing

#### 2. **Standard Library Expansion**
- [ ] Math/linear algebra, file/network I/O, parallel/distributed utils
- [ ] GPU/CPU-accelerated numerical and graphics routines

#### 3. **Ecosystem & Tooling**
- [ ] Write package manager for Tether libraries
- [ ] Develop VSCode/IDE plugin for syntax highlighting, linting, REPL
- [ ] Write documentation generator

---

### Phase 4: Self-Hosting

- [ ] Reimplement core parts of the interpreter/compiler in Tether
- [ ] Gradually phase out C in favor of Tether code
- [ ] Ship a bootstrap process (building Tether from source)

---

### Phase 5: Community & Releases

- [ ] Write comprehensive docs and tutorials
- [ ] Release first alpha (with examples, benchmarks, and showcase projects)
- [ ] Build community forum/Discord/Matrix for discussion and support
- [ ] Collect feedback and iterate

---

## ✅ Progress Checklist

- [ ] Project planning and research
- [ ] Minimal syntax and interpreter in C
- [ ] REPL and basic stdlib
- [ ] Parallel CPU execution
- [ ] GPU offload (basic)
- [ ] Distributed node execution (basic)
- [ ] Standard library buildout
- [ ] Self-hosting
- [ ] Docs, tooling, and first public release

---

## 🙏 Contributions

Pull requests and design feedback are welcome at all stages! See `CONTRIBUTING.md` for details.

---