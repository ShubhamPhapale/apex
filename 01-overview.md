# 1. Language Overview

## Language Name

**Apex** (Advanced Programming for Explicit eXecution)

The name reflects the language's goal to reach the peak of systems programming by combining low-level control with high-level safety.

## Design Goals

### Primary Goals

1. **Preserve C's Low-Level Power**
   - Direct memory access and manipulation
   - Zero-cost abstractions
   - Predictable performance characteristics
   - No hidden runtime overhead
   - Bare-metal capability

2. **Enhance Safety Without Sacrificing Performance**
   - Memory safety by default with explicit unsafe escapes
   - Compile-time null pointer checking
   - Bounds checking with optimization opportunities
   - Data race detection at compile time
   - No garbage collection overhead

3. **Improve Developer Experience**
   - Clear, readable syntax
   - Excellent error messages
   - Modern tooling (LSP, debugger, formatter)
   - Fast compilation times
   - Comprehensive standard library

4. **Maintain C Compatibility**
   - Direct C FFI without wrapper overhead
   - Ability to call and be called by C code
   - Compatible with existing build systems
   - Can replace C incrementally in projects

5. **Support Modern Programming Paradigms**
   - Module system instead of header files
   - Algebraic data types (enums with data)
   - Pattern matching
   - Generic programming
   - First-class error handling

## Target Use-Cases

Apex is designed for domains where performance, control, and reliability are critical:

### Systems Programming
- Operating system kernels and drivers
- File systems and storage systems
- Virtual machines and interpreters
- Network protocol implementations
- System daemons and services

### Embedded Development
- Microcontroller firmware
- Real-time operating systems (RTOS)
- IoT devices
- Automotive systems
- Industrial control systems

### High-Performance Computing
- Scientific computing applications
- Signal processing
- Computer graphics and rendering
- Physics simulations
- High-frequency trading systems

### Game Development
- Game engines
- Physics engines
- Audio processing
- Asset pipeline tools
- Platform abstraction layers

### Infrastructure Software
- Databases
- Web servers and reverse proxies
- Container runtimes
- Distributed systems
- Network tools and utilities

## Main Philosophy

### "Explicit over Implicit"

Apex never hides costs from the programmer. Every operation that allocates memory, performs I/O, or has runtime overhead is explicit in the code.

**Examples:**
- Memory allocation requires explicit keywords (`alloc`, `dealloc`)
- No implicit conversions that could lose data
- No hidden function calls (operator overloading is explicit)
- Clear distinction between copying and moving

### "Safe by Default, Unsafe When Needed"

The language provides safety guarantees in safe mode but allows programmers to opt into unsafe operations when necessary for performance or interfacing with hardware.

**Safe Mode Guarantees:**
- No null pointer dereferences
- No buffer overflows
- No use-after-free
- No data races
- No undefined behavior

**Unsafe Mode Capabilities:**
- Raw pointer arithmetic
- Direct memory manipulation
- Inline assembly
- Type punning
- FFI with C

### "Zero-Cost Abstractions"

High-level features compile down to the same machine code you would write by hand. Abstraction should not impose runtime overhead.

**Implementation:**
- Generics via monomorphization
- Inlining of small functions
- Compile-time computation
- Dead code elimination
- Static dispatch by default

### "Clear over Clever"

Code should be easy to read and understand. Clever tricks that obscure intent are discouraged in favor of explicit, clear code.

**Guidelines:**
- Descriptive naming conventions
- Limited operator overloading
- No hidden control flow
- Explicit error propagation
- Clear lifetime annotations

## Language Classification

- **Paradigm**: Imperative, procedural, with functional features
- **Typing**: Static, strong, with type inference
- **Memory Management**: Manual with safety checks
- **Compilation**: Ahead-of-time (AOT) to native code
- **Performance**: Zero-overhead abstractions
- **Safety**: Memory-safe in safe mode, unsafe blocks for low-level operations

## Key Differentiators from C

| Aspect | C | Apex |
|--------|---|------|
| Memory Safety | None | Compile-time checked in safe mode |
| Null Pointers | Allowed everywhere | Must use Optional type or unsafe |
| Header Files | Required | Module system, no headers |
| Error Handling | Error codes, errno | Result<T, E> type with pattern matching |
| Type System | Weak | Strong with inference |
| Generics | None (macros) | True generics with monomorphization |
| Undefined Behavior | Common | Explicit unsafe blocks only |
| String Type | char* | Safe string type with UTF-8 support |
| Concurrency | None | Built-in threading primitives |

## Design Inspirations

Apex draws inspiration from several languages while maintaining its unique identity:

- **C**: Core syntax, performance model, systems programming focus
- **Rust**: Memory safety model, ownership concepts, Result types
- **Zig**: Explicit allocation, comptime, C interop approach
- **Go**: Simplicity, fast compilation, clear error handling
- **Swift**: Clean syntax, optional types, strong type system

## Success Criteria

Apex will be considered successful if it achieves:

1. **Performance**: Within 5% of hand-optimized C in benchmarks
2. **Safety**: Elimination of 90%+ of memory safety bugs at compile time
3. **Adoption**: Used in at least one major open-source systems project
4. **Tooling**: IDE support via LSP, integrated debugger, package manager
5. **Ecosystem**: Growing standard library and third-party package ecosystem
6. **Compatibility**: Seamless C interop demonstrated in real-world projects

## Who Should Use Apex?

### Ideal For:
- C programmers wanting better safety and tooling
- Rust programmers needing simpler syntax and faster compile times
- Systems programmers building performance-critical applications
- Embedded developers requiring precise control
- Game developers needing predictable performance

### Not Ideal For:
- Rapid application development (use Python, JavaScript)
- Web frontend development (use TypeScript)
- Applications where GC is acceptable (use Go, Java, C#)
- Projects requiring maximum ecosystem maturity (use C++)

## Language Stability Promise

- **Syntax Stability**: Version 1.0 syntax will remain valid indefinitely
- **ABI Stability**: Binary compatibility within major versions
- **Breaking Changes**: Only in major version updates with migration tools
- **Deprecation Policy**: Minimum 2 years before removal with warnings
- **Long-term Support**: LTS releases supported for 5 years

---

**Next**: [02-features.md](02-features.md) - Comprehensive Feature Specification
