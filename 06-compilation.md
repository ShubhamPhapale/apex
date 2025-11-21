# 6. Compilation Workflow and Architecture

## Overview

The Apex compiler (`apexc`) is designed as a multi-phase compiler with emphasis on fast incremental compilation, excellent error messages, and aggressive optimization capabilities.

## Source File Organization

### File Extensions

- `.apex` - Apex source files
- `.apexmod` - Compiled module metadata
- `.o` - Object files
- `.a` / `.lib` - Static libraries
- `.so` / `.dll` / `.dylib` - Dynamic libraries

### Project Structure

```
my_project/
├── project.apex          # Project configuration
├── src/
│   ├── main.apex        # Entry point
│   ├── lib.apex         # Library root
│   └── module/
│       ├── mod.apex     # Module declaration
│       ├── types.apex   # Module subfile
│       └── utils.apex   # Module subfile
├── tests/
│   ├── unit_tests.apex
│   └── integration/
│       └── test_api.apex
├── examples/
│   └── demo.apex
├── build/               # Compiler output (generated)
│   ├── debug/
│   └── release/
└── target/              # Final artifacts (generated)
    ├── debug/
    │   └── my_project
    └── release/
        └── my_project
```

## Compilation Phases

### Phase 1: Lexical Analysis (Lexer)

**Purpose:** Convert source text into tokens

**Input:** Raw source code (UTF-8 text)  
**Output:** Token stream

**Operations:**
1. Read source file character by character
2. Recognize keywords, identifiers, literals, operators
3. Skip whitespace and comments
4. Track source locations (file, line, column) for error reporting

**Example:**
```apex
fn add(a: i32, b: i32) -> i32 { return a + b; }
```

**Tokens:**
```
KEYWORD(fn) IDENTIFIER(add) LPAREN
IDENTIFIER(a) COLON KEYWORD(i32) COMMA
IDENTIFIER(b) COLON KEYWORD(i32) RPAREN
ARROW KEYWORD(i32)
LBRACE KEYWORD(return) IDENTIFIER(a) PLUS IDENTIFIER(b) SEMICOLON RBRACE
```

**Error Handling:**
- Invalid UTF-8 sequences
- Unknown characters
- Unterminated strings/comments
- Invalid number formats

### Phase 2: Syntactic Analysis (Parser)

**Purpose:** Build Abstract Syntax Tree (AST)

**Input:** Token stream  
**Output:** AST (typed tree structure)

**Operations:**
1. Recursive descent parsing
2. Construct AST nodes for each language construct
3. Check syntax validity
4. Maintain source location information

**AST Structure:**
```
FunctionDecl {
    name: "add",
    params: [
        Param { name: "a", type: i32 },
        Param { name: "b", type: i32 }
    ],
    return_type: i32,
    body: Block {
        stmts: [
            Return {
                expr: BinaryOp {
                    op: Add,
                    left: Identifier("a"),
                    right: Identifier("b")
                }
            }
        ]
    }
}
```

**Error Recovery:**
- Synchronize on statement boundaries
- Continue parsing after errors
- Report multiple errors in one pass

### Phase 3: Semantic Analysis

**Purpose:** Type checking, name resolution, and semantic validation

**Input:** AST  
**Output:** Typed AST (HIR - High-level Intermediate Representation)

**Sub-phases:**

#### 3.1 Name Resolution
- Resolve identifiers to definitions
- Build symbol tables
- Check for duplicate definitions
- Resolve imports and module paths

```apex
// Example: Resolving imports
import std.io;

fn main() {
    io.println("Hello");  // Resolve io → std.io
}
```

#### 3.2 Type Inference
- Infer types where not explicitly specified
- Unification algorithm for generic types
- Constraint solving

```apex
let x = 42;         // Infer x: i32
let y = x + 1.0;   // Error: type mismatch
```

#### 3.3 Type Checking
- Verify type correctness
- Check function signatures
- Validate trait bounds
- Ensure memory safety rules

#### 3.4 Borrow Checking
- Analyze ownership and borrowing
- Check lifetime constraints
- Verify no data races
- Ensure references are valid

**Error Examples:**
```apex
// Use after move
let s1 = String.from("hello");
let s2 = s1;
println("{}", s1);  // ERROR: s1 moved

// Multiple mutable borrows
let mut x = 5;
let r1 = &mut x;
let r2 = &mut x;    // ERROR: cannot borrow mutably twice
```

#### 3.5 Safety Checking
- Verify array bounds (when possible)
- Check for potential null dereferences
- Validate unsafe block usage
- Detect unreachable code

### Phase 4: MIR (Mid-level Intermediate Representation)

**Purpose:** Simplified representation for optimization and analysis

**Input:** HIR (typed AST)  
**Output:** MIR (control-flow graph)

**Characteristics:**
- Desugared (no syntactic sugar)
- Explicit control flow
- Explicit drops (destructors)
- Basic blocks and CFG (Control Flow Graph)

**MIR Example:**
```apex
// Source
fn max(a: i32, b: i32) -> i32 {
    if a > b { a } else { b }
}

// MIR (simplified)
fn max(a: i32, b: i32) -> i32 {
    bb0: {
        let temp = a > b;
        if temp goto bb1 else goto bb2;
    }
    bb1: {
        return a;
    }
    bb2: {
        return b;
    }
}
```

**MIR Optimizations:**
- Dead code elimination
- Constant propagation
- Common subexpression elimination
- Loop-invariant code motion
- Function inlining

### Phase 5: LLVM IR Generation

**Purpose:** Convert to LLVM Intermediate Representation

**Input:** MIR  
**Output:** LLVM IR

**Operations:**
1. Map Apex types to LLVM types
2. Generate LLVM instructions
3. Emit function declarations
4. Add metadata (debug info, attributes)

**LLVM IR Example:**
```llvm
define i32 @add(i32 %a, i32 %b) {
entry:
  %result = add i32 %a, %b
  ret i32 %result
}
```

**LLVM Benefits:**
- World-class optimizer
- Multiple backend targets
- Extensive tooling ecosystem
- Active development

### Phase 6: LLVM Optimization

**Purpose:** Aggressive optimization

**Input:** LLVM IR  
**Output:** Optimized LLVM IR

**Optimization Levels:**

| Level | Description | Compile Time | Runtime Performance |
|-------|-------------|--------------|---------------------|
| `-O0` | No optimization (debug) | Fast | Slow |
| `-O1` | Basic optimization | Medium | Good |
| `-O2` | Standard optimization | Slow | Very Good |
| `-O3` | Aggressive optimization | Very Slow | Excellent |
| `-Os` | Optimize for size | Medium | Good |
| `-Oz` | Aggressively optimize for size | Medium | Good |

**Key Optimizations:**
- Function inlining
- Loop unrolling and vectorization
- Dead code elimination
- Constant folding
- Tail call optimization
- Devirtualization
- Link-time optimization (LTO)

### Phase 7: Code Generation (Backend)

**Purpose:** Generate native machine code

**Input:** Optimized LLVM IR  
**Output:** Object files (`.o`)

**Operations:**
1. Instruction selection
2. Register allocation
3. Instruction scheduling
4. Machine code emission

**Target-Specific:**
- x86-64: SSE, AVX instructions
- ARM: NEON instructions
- RISC-V: Compressed instructions
- WebAssembly: WebAssembly instructions

### Phase 8: Linking

**Purpose:** Combine object files into executables or libraries

**Input:** Object files + libraries  
**Output:** Executable or library

**Linker Types:**
- **Static linking:** Embed all dependencies
- **Dynamic linking:** Link at runtime
- **LTO (Link-Time Optimization):** Cross-module optimization

**Linker Flags:**
```bash
# Static linking
apexc --link-static -o my_program src/main.apex

# Dynamic linking
apexc --link-dynamic -o my_program src/main.apex

# LTO
apexc --lto -o my_program src/main.apex
```

## Compiler Architecture

### High-Level Architecture

```
┌─────────────────┐
│  Source Files   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     Lexer       │ ◄── Error Recovery
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     Parser      │ ◄── Error Recovery
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Name Resolver  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Type Checker   │ ◄── Type Inference Engine
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Borrow Checker  │ ◄── Ownership Analysis
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   MIR Builder   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ MIR Optimizer   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  LLVM IR Gen    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ LLVM Optimizer  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Code Gen       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     Linker      │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   Executable    │
└─────────────────┘
```

### Incremental Compilation

**Strategy:** Only recompile changed modules

**Implementation:**
1. Track file modification times
2. Store module dependency graph
3. Invalidate dependent modules
4. Reuse unchanged modules

**Metadata Files (`.apexmod`):**
```
module_name: std.collections.vec
dependencies: [std.mem, std.alloc]
exported_items: [Vec, VecIterator]
hash: 0xabcd1234
timestamp: 2025-01-15T10:30:00Z
```

**Build Process:**
```bash
# First build
apexc build src/main.apex
# Compiles: main.apex, module_a.apex, module_b.apex

# Edit module_b.apex
# Second build
apexc build src/main.apex
# Compiles: module_b.apex, main.apex (depends on module_b)
# Reuses: module_a.apex (unchanged)
```

### Parallel Compilation

**Strategy:** Compile independent modules in parallel

**Implementation:**
1. Build dependency graph
2. Identify independent modules
3. Spawn worker threads
4. Distribute compilation tasks

**Example:**
```
Module Graph:
    main
   /    \
  A      B
  |      |
  C      D

Compilation Order:
Wave 1: C, D (parallel)
Wave 2: A, B (parallel, after C and D)
Wave 3: main (after A and B)
```

## Target Platforms

### Supported Targets

| Architecture | Operating System | Status | Notes |
|--------------|------------------|--------|-------|
| x86-64 | Linux | Tier 1 | Primary platform |
| x86-64 | Windows | Tier 1 | Native support |
| x86-64 | macOS | Tier 1 | Native support |
| ARM64 | Linux | Tier 1 | aarch64 |
| ARM64 | macOS | Tier 1 | Apple Silicon |
| ARM64 | Android | Tier 2 | Mobile support |
| ARM | Linux | Tier 2 | ARMv7 embedded |
| RISC-V | Linux | Tier 2 | rv64gc |
| WebAssembly | Browser | Tier 2 | wasm32 |
| x86 | Linux | Tier 3 | 32-bit legacy |

**Tier Definitions:**
- **Tier 1:** Guaranteed to work, tested in CI, official releases
- **Tier 2:** Guaranteed to build, may have limitations
- **Tier 3:** Build support only, may not fully work

### Cross-Compilation

```bash
# Compile for different target
apexc --target=aarch64-linux-gnu -o my_program src/main.apex

# List available targets
apexc --print-targets

# Cross-compile with sysroot
apexc --target=arm-linux-gnueabihf \
      --sysroot=/usr/arm-linux-gnueabihf \
      -o my_program src/main.apex
```

### Bare-Metal Targets

```apex
// project.apex
project {
    name: "embedded_app",
    target: "thumbv7em-none-eabihf",  // Cortex-M4
    
    build: {
        no_std: true,          // No standard library
        panic: "abort",        // Panic behavior
        linker_script: "memory.ld",
    },
}
```

## Compatibility with Existing Toolchains

### C Interoperability

**Calling C from Apex:**
```apex
extern "C" {
    fn malloc(size: usize) -> *mut void;
    fn free(ptr: *mut void);
    fn printf(format: *const char, ...) -> i32;
}

fn use_c_library() {
    unsafe {
        let ptr = malloc(100);
        if ptr != null {
            printf(c"Allocated at %p\n", ptr);
            free(ptr);
        }
    }
}
```

**Calling Apex from C:**
```apex
// Apex code
#[no_mangle]
extern "C" fn add_numbers(a: i32, b: i32) -> i32 {
    return a + b;
}
```

```c
// C code
extern int add_numbers(int a, int b);

int main() {
    int result = add_numbers(5, 3);
    printf("Result: %d\n", result);
    return 0;
}
```

### LLVM Integration

**Benefits:**
- Leverage LLVM's optimization infrastructure
- Access to all LLVM backends
- Compatible with LLVM tooling (profilers, sanitizers)
- Interoperate with other LLVM-based languages

**Custom Passes:**
```apex
// Compiler plugin for custom optimization
#[compiler_plugin]
fn custom_optimization_pass(module: &mut llvm.Module) {
    // Implement custom LLVM IR transformation
}
```

### Build System Integration

**CMake:**
```cmake
find_package(Apex REQUIRED)

apex_add_executable(my_program
    src/main.apex
    src/module.apex
)

apex_link_libraries(my_program
    std::io
    std::net
)
```

**Make:**
```makefile
APEXC = apexc
APEX_FLAGS = -O2

my_program: src/main.apex src/module.apex
	$(APEXC) $(APEX_FLAGS) -o $@ $^
```

**Cargo (Rust ecosystem):**
```toml
[dependencies]
my_apex_lib = { path = "../my_apex_lib", language = "apex" }
```

## Debugging Model

### Debug Information

**DWARF Format:**
- Standard debug format (DWARF 4/5)
- Compatible with GDB, LLDB
- Source-level debugging
- Variable inspection
- Stack traces

**Generating Debug Info:**
```bash
# Full debug info
apexc -g -o my_program src/main.apex

# Optimized with debug info
apexc -O2 -g -o my_program src/main.apex
```

### Debugger Integration

**GDB:**
```bash
$ gdb my_program
(gdb) break main
(gdb) run
(gdb) print my_variable
(gdb) backtrace
```

**LLDB:**
```bash
$ lldb my_program
(lldb) breakpoint set --name main
(lldb) run
(lldb) frame variable
(lldb) thread backtrace
```

### Pretty Printers

Custom pretty printers for complex types:

```python
# apex_gdb.py - GDB pretty printer
class ApexStringPrinter:
    def __init__(self, val):
        self.val = val
    
    def to_string(self):
        ptr = self.val['ptr']
        len = self.val['len']
        return ptr.string('utf-8', 'ignore', len)

def register_apex_printers(objfile):
    objfile.pretty_printers.append(lookup_apex_type)
```

### Sanitizers

**AddressSanitizer (ASan):**
```bash
apexc --sanitize=address -o my_program src/main.apex
./my_program  # Detects memory errors
```

**ThreadSanitizer (TSan):**
```bash
apexc --sanitize=thread -o my_program src/main.apex
./my_program  # Detects data races
```

**UndefinedBehaviorSanitizer (UBSan):**
```bash
apexc --sanitize=undefined -o my_program src/main.apex
./my_program  # Detects undefined behavior
```

**MemorySanitizer (MSan):**
```bash
apexc --sanitize=memory -o my_program src/main.apex
./my_program  # Detects uninitialized reads
```

## Compiler Invocation

### Basic Usage

```bash
# Compile and link
apexc -o my_program src/main.apex

# Compile only (no linking)
apexc -c -o main.o src/main.apex

# Check syntax only
apexc --check src/main.apex

# Build project
apexc build

# Run directly
apexc run src/main.apex
```

### Optimization Flags

```bash
# Debug (default)
apexc -o my_program src/main.apex

# Release (optimized)
apexc -O2 -o my_program src/main.apex

# Maximum optimization
apexc -O3 --lto -o my_program src/main.apex

# Size optimization
apexc -Os --strip -o my_program src/main.apex
```

### Feature Flags

```bash
# Enable/disable features
apexc --feature networking --feature threading -o my_program src/main.apex

# All features
apexc --all-features -o my_program src/main.apex

# No default features
apexc --no-default-features -o my_program src/main.apex
```

### Diagnostic Options

```bash
# Warnings as errors
apexc -Werror -o my_program src/main.apex

# Show all warnings
apexc -Wall -o my_program src/main.apex

# Verbose output
apexc -v -o my_program src/main.apex

# Print compilation times
apexc --time-passes -o my_program src/main.apex

# Explain error codes
apexc --explain E0308
```

## Compilation Performance

### Benchmarks (Indicative)

| Project Size | Lines of Code | Debug Build | Release Build | Incremental |
|--------------|---------------|-------------|---------------|-------------|
| Small | 1K | 0.5s | 2s | 0.2s |
| Medium | 10K | 3s | 15s | 1s |
| Large | 100K | 25s | 120s | 5s |
| Very Large | 1M | 200s | 1200s | 30s |

**Comparison with Other Languages:**

| Language | Small Project | Medium Project | Large Project |
|----------|---------------|----------------|---------------|
| Apex | 0.5s | 3s | 25s |
| C (gcc) | 0.3s | 2s | 20s |
| C++ (g++) | 1s | 8s | 80s |
| Rust | 2s | 15s | 150s |
| Go | 0.2s | 1s | 10s |

### Optimization Strategies

**For Fast Compilation:**
1. Use incremental compilation
2. Enable parallel compilation
3. Minimize dependencies
4. Use pipelined compilation
5. Cache compiled modules

**For Fast Runtime:**
1. Use `-O3` optimization
2. Enable LTO (Link-Time Optimization)
3. Use PGO (Profile-Guided Optimization)
4. Target-specific optimizations

## Build Configuration

### project.apex

```apex
project {
    name: "my_application",
    version: "1.0.0",
    authors: ["Alice <alice@example.com>"],
    license: "MIT",
    
    dependencies: {
        "std": "1.0",
        "network": { version: "2.1", features: ["ssl"] },
        "json": { git: "https://github.com/apex-lang/json" },
    },
    
    targets: {
        executable: {
            name: "my_app",
            entry: "src/main.apex",
        },
        
        library: {
            name: "my_lib",
            entry: "src/lib.apex",
            crate_type: "staticlib",  // or "dylib"
        },
    },
    
    build: {
        optimization: "release",  // "debug" | "release"
        debug_symbols: true,
        lto: true,
        codegen_units: 16,
        
        features: ["networking", "threading"],
        default_features: true,
    },
    
    profiles: {
        dev: {
            optimization: 0,
            debug: true,
        },
        release: {
            optimization: 3,
            debug: false,
            lto: true,
        },
        bench: {
            optimization: 3,
            debug: true,
        },
    },
}
```

---

**Next**: [07-stdlib.md](07-stdlib.md) - Standard Library Specification
