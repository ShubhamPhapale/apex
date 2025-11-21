# Changelog

All notable changes to the Apex programming language and compiler will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Advanced borrow checker implementation
- Generic monomorphization
- Complete standard library
- LSP server for IDE support
- Code formatter and linter
- Comprehensive test suite

## [1.0.0-alpha] - 2025-01-XX

### Added
- Complete language specification (10 documents)
- C++20 compiler implementation with LLVM backend
- Lexer with full tokenization support
  - All keywords (fn, let, struct, enum, etc.)
  - Operators (arithmetic, logical, bitwise)
  - Literals (integers, floats, strings, chars)
  - Comments (line and block, nested blocks)
  - Source location tracking for error reporting
- Recursive descent parser
  - Expression parsing with operator precedence
  - Statement parsing (let, expressions)
  - Item parsing (functions, structs, enums, traits, impl blocks)
  - Pattern matching syntax
  - Generic parameter support
- AST (Abstract Syntax Tree)
  - Complete type system representation
  - Expression nodes (binary, unary, call, index, field access, etc.)
  - Statement nodes (let, expression, item)
  - Item nodes (functions, structs, enums, traits, impl, modules)
  - Pattern matching structures
- Basic semantic analysis
  - Symbol table and scope management
  - Name resolution
  - Basic type checking
  - Duplicate detection (fields, variants, parameters)
- LLVM code generation
  - Function code generation
  - Struct type generation
  - Expression compilation (arithmetic, logical, comparisons)
  - Control flow (if-else expressions)
  - Function calls
  - Object file emission
  - LLVM IR emission
- Compiler driver (apexc)
  - Command-line interface
  - Multiple output formats (object files, LLVM IR, AST, tokens)
  - Verbose mode for debugging
  - Proper error reporting with source locations
- Example programs
  - hello.apx - Simple hello world
  - fibonacci.apx - Recursive fibonacci
  - struct.apx - Struct usage example
- Build system
  - CMake configuration for all platforms
  - Build script (build.sh) for quick building
  - Test script (test.sh) for automated testing
- Documentation
  - BUILD.md - Comprehensive build instructions
  - CONTRIBUTING.md - Contribution guidelines
  - Complete language specification files
  - Updated README with quick start guide

### Compiler Features
- Target platforms: x86-64, ARM64
- Operating systems: Linux, macOS, Windows (via WSL)
- LLVM version: 14+
- C++ standard: C++20

### Language Features (Specified)
- Primitive types: i8-i128, u8-u128, f32/f64, bool, char
- Composite types: structs, enums, tuples, arrays
- Functions with return types
- Pattern matching
- Modules and imports
- Generic types (specification complete, implementation pending)
- Ownership and borrowing (specification complete, implementation pending)
- Safe/unsafe blocks (specification complete, implementation pending)
- Traits (specification complete, implementation pending)

### Known Limitations
- Standard library not yet implemented
- Borrow checker not yet enforced
- Generics parsing only, no monomorphization yet
- Pattern matching parsing only, no exhaustiveness checking
- No optimization passes yet
- Limited error recovery in parser
- No incremental compilation

### Build Requirements
- CMake 3.20 or later
- C++20 compatible compiler:
  - GCC 10+
  - Clang 13+
  - MSVC 19.29+
- LLVM 14+ with development headers
- Standard build tools (make/ninja)

### Platform Support
- ✅ macOS (Intel and Apple Silicon)
- ✅ Linux (Ubuntu 22.04+, Debian, Fedora, Arch)
- ✅ Windows via WSL2

## [0.1.0] - 2025-01-XX (Specification Only)

### Added
- Initial language specification
- Design documents
- Syntax examples
- Feature proposals

---

## Version History

- **1.0.0-alpha**: First compiler implementation (current)
- **0.1.0**: Initial specification release

## Upcoming Milestones

### v1.0.0-beta (Target: Q2 2025)
- Complete borrow checker
- Generic monomorphization
- Basic standard library (I/O, collections)
- Comprehensive test suite

### v1.0.0 (Target: Q4 2025)
- Production-ready compiler
- Complete standard library
- LSP server
- Package manager
- Full documentation

For more details, see [docs/spec/10-roadmap.md](docs/spec/10-roadmap.md).

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex
