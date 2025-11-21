# Apex Programming Language

**A Modern Systems Programming Language for the Next Generation**

Apex is a C-like systems programming language designed to preserve the low-level control and performance characteristics of C while dramatically improving safety, tooling, and developer experience. Built for systems programming, embedded development, OS kernels, game engines, and high-performance computing.

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/apex-lang/apex.git
cd apex

# Build the compiler (requires LLVM 14+)
./build.sh

# Compile your first program
./build/src/apexc/apexc examples/hello.apex

# Compile with verbose output
./build/src/apexc/apexc -v examples/fibonacci.apex
```

## 📖 Documentation Structure

This repository contains the complete specification for the Apex programming language:

1. **[01-overview.md](01-overview.md)** - Language overview, design goals, and philosophy
2. **[02-features.md](02-features.md)** - Comprehensive feature specification
3. **[03-syntax.md](03-syntax.md)** - Syntax specification with examples
4. **[04-grammar.md](04-grammar.md)** - Formal EBNF grammar
5. **[05-memory-model.md](05-memory-model.md)** - Memory model and safety rules
6. **[06-compilation.md](06-compilation.md)** - Compilation workflow and architecture
7. **[07-stdlib.md](07-stdlib.md)** - Standard library specification
8. **[08-examples.md](08-examples.md)** - Complete example applications
9. **[09-comparison.md](09-comparison.md)** - Language comparison matrix
10. **[10-roadmap.md](10-roadmap.md)** - Future evolution roadmap

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
./build/src/apexc/apexc examples/hello.apex
./build/src/apexc/apexc examples/fibonacci.apex
./build/src/apexc/apexc examples/struct.apex
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
├── examples/            # Example Apex programs
├── docs/                # Additional documentation
├── CMakeLists.txt       # Root build configuration
├── build.sh             # Build script
├── test.sh              # Test script
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

**Status**: Alpha - Core compiler functional, standard library in progress  
**License**: MIT  
**Version**: 1.0.0-alpha
