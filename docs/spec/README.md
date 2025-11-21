# Apex Language Specification

This directory contains the complete formal specification for the Apex programming language.

## Reading Guide

The specification is organized into 10 documents that should be read in order:

### Part I: Language Fundamentals

1. **[01-overview.md](01-overview.md)** - Start here!
   - Language philosophy and design goals
   - Key differentiators from C, C++, and Rust
   - Target use cases and audience

2. **[02-features.md](02-features.md)** - Core language features
   - Type system
   - Ownership and borrowing
   - Pattern matching
   - Error handling
   - Concurrency primitives

3. **[03-syntax.md](03-syntax.md)** - Syntax with examples
   - Variables and types
   - Control flow
   - Functions
   - Structs and enums
   - Code examples for every feature

4. **[04-grammar.md](04-grammar.md)** - Formal EBNF grammar
   - Complete language grammar
   - Token definitions
   - Precedence rules
   - Used for parser implementation

### Part II: Advanced Concepts

5. **[05-memory-model.md](05-memory-model.md)** - Memory management
   - Stack vs heap allocation
   - Ownership rules
   - Borrowing and lifetimes
   - Move vs copy semantics
   - Unsafe operations

6. **[06-compilation.md](06-compilation.md)** - Compiler architecture
   - Compilation pipeline
   - Module system
   - Build configuration
   - Optimization levels
   - Target architectures

7. **[07-stdlib.md](07-stdlib.md)** - Standard library
   - Core modules (io, fs, mem, collections)
   - API reference
   - Usage examples
   - Platform-specific APIs

### Part III: Practical Information

8. **[08-examples.md](08-examples.md)** - Complete applications
   - Real-world examples
   - CLI tools
   - Network servers
   - Embedded systems
   - Game development
   - Systems programming

9. **[09-comparison.md](09-comparison.md)** - Language comparison
   - Apex vs C
   - Apex vs C++
   - Apex vs Rust
   - Apex vs Go
   - Feature comparison matrix
   - Migration guides

10. **[10-roadmap.md](10-roadmap.md)** - Future development
    - Version 1.0 goals
    - Version 2.0 features
    - Long-term vision
    - Stability guarantees
    - Community growth

## Quick Reference

For a condensed cheat sheet, see [../QUICKREF.md](../QUICKREF.md).

## Implementation Status

For current implementation progress, see [../STATUS.md](../STATUS.md).

## Contributing

To contribute to the specification or implementation, see [../../CONTRIBUTING.md](../../CONTRIBUTING.md).

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  
**Version**: 1.0.0-alpha
