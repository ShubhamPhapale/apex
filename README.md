# Apex Systems Programming Language

**Low-Level Control with Modern Safety Guarantees**

> **Note**: This is an independent systems programming language project. Not affiliated with or related to apexlang.io or any other Apex-named projects.

Apex is a statically-typed systems programming language that combines C's low-level control with modern safety features. Designed for operating systems, embedded systems, device drivers, and performance-critical applications where manual memory management and zero-cost abstractions are essential.

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/ShubhamPhapale/apex.git
cd apex

# Build the compiler (requires LLVM 14+)
./build.sh

# Compile your first program
./build/src/apexc/apexc examples/hello.apx

# Compile with verbose output
./build/src/apexc/apexc -v examples/fibonacci.apx
```

## 📖 Documentation Structure

This repository contains the complete specification for the Apex programming language:

### Core Documentation
- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Contribution guidelines
- **[CHANGELOG.md](CHANGELOG.md)** - Version history and changes
- **[docs/BUILD.md](docs/BUILD.md)** - Build instructions
- **[docs/USAGE.md](docs/USAGE.md)** - Usage guide
- **[docs/QUICKREF.md](docs/QUICKREF.md)** - Quick reference
- **[docs/STATUS.md](docs/STATUS.md)** - Implementation status

### Language Specification
1. **[01-overview.md](docs/spec/01-overview.md)** - Language overview, design goals, and philosophy
2. **[02-features.md](docs/spec/02-features.md)** - Comprehensive feature specification
3. **[03-syntax.md](docs/spec/03-syntax.md)** - Syntax specification with examples
4. **[04-grammar.md](docs/spec/04-grammar.md)** - Formal EBNF grammar
5. **[05-memory-model.md](docs/spec/05-memory-model.md)** - Memory model and safety rules
6. **[06-compilation.md](docs/spec/06-compilation.md)** - Compilation workflow and architecture
7. **[07-stdlib.md](docs/spec/07-stdlib.md)** - Standard library specification
8. **[08-examples.md](docs/spec/08-examples.md)** - Complete example applications
9. **[09-comparison.md](docs/spec/09-comparison.md)** - Language comparison matrix
10. **[10-roadmap.md](docs/spec/10-roadmap.md)** - Future evolution roadmap

## Quick Start

```apex
// Hello World in Apex
import std.io;

fn main() -> i32 {
    io.println("Hello, World!");
    return 0;
}
```

## Key Features

- **Zero-cost abstractions** - Modern features with no runtime overhead
- **Memory safety modes** - Safe by default, unsafe when needed
- **Module system** - No more header files
- **Compile-time guarantees** - Catch errors before runtime
- **C interoperability** - Seamless integration with existing C code
- **Explicit control** - You decide when allocation happens
- **Modern error handling** - Result types with pattern matching
- **Built-in concurrency** - First-class thread support

## Philosophy

**"The power of C with the safety of modern languages"**

Apex is built on three core principles:

1. **Explicit over implicit** - No hidden costs or surprises
2. **Safe by default** - Opt into unsafe operations explicitly
3. **Zero compromise** - Modern features without sacrificing performance

## 🛠️ Compiler Implementation

The Apex compiler (`apexc`) is implemented in C++20 with LLVM backend:

**Architecture:**
- **Lexer** - Token stream generation with full Unicode support
- **Parser** - Recursive descent parser generating typed AST
- **Semantic Analyzer** - Name resolution, type checking, borrow checking
- **LLVM Codegen** - Native code generation for x86-64 and ARM64

**Build Instructions:**
See **[docs/BUILD.md](docs/BUILD.md)** for detailed build instructions.

**Usage:**
```bash
apexc [options] <input-file>

Options:
  -o <file>          Write output to <file>
  --emit-llvm        Emit LLVM IR instead of object file
  --emit-ast         Print the AST and exit
  --emit-tokens      Print tokens and exit
  -v, --verbose      Enable verbose output
  -h, --help         Display help message
```

## 🧪 Testing

```bash
# Run test suite
./test.sh

# Test individual examples
./build/src/apexc/apexc examples/hello.apx
./build/src/apexc/apexc examples/fibonacci.apx
./build/src/apexc/apexc examples/struct.apx
```

## 📂 Project Structure

```
apex/
├── src/apexc/           # Compiler implementation
│   ├── lexer/           # Tokenization
│   ├── parser/          # Parsing & AST
│   ├── ast/             # AST definitions
│   ├── sema/            # Semantic analysis
│   ├── codegen/         # LLVM code generation
│   └── main.cpp         # Compiler driver
├── docs/                # Documentation
│   ├── spec/            # Language specification (01-10)
│   ├── BUILD.md         # Build instructions
│   ├── STATUS.md        # Implementation status
│   ├── USAGE.md         # Usage guide
│   ├── QUICKREF.md      # Quick reference
│   └── IMPLEMENTATION_SUMMARY.md
├── examples/            # Example programs (.apx files)
├── tests/               # Test suite
├── CMakeLists.txt       # Root build configuration
├── build.sh             # Build script
├── test.sh              # Test script
├── CONTRIBUTING.md      # Contribution guidelines
├── CHANGELOG.md         # Version history
└── README.md            # This file
```

## 🎯 Implementation Status

**✅ Completed:**
- Language specification (complete)
- Lexer with full token support
- Parser generating complete AST
- Basic semantic analysis
- LLVM code generation
- Example programs

**🔄 In Progress:**
- Advanced borrow checker
- Generic monomorphization
- Standard library runtime
- Comprehensive test suite

**⏳ Planned:**
- LSP server for IDE support
- Code formatter (apexfmt)
- Linter (apexlint)
- Package manager
- Documentation generator

---

**Author**: Shubham Phapale  
**GitHub**: https://github.com/ShubhamPhapale/apex  
**Status**: Alpha - Core compiler functional, standard library in progress  
**License**: MIT  
**Version**: 1.0.0-alpha
