# How to Compile and Run Apex Programs

## Prerequisites

Make sure you have:
- LLVM 20 installed (`brew install llvm`)
- Clang compiler
- CMake

## Building the Compiler

```bash
cd "/Users/shubhamsarjeraophapale/Random Projects/New Programming Language"
./build.sh
```

This creates the compiler at `build/src/apexc/apexc`.

## Compiling and Running Your Program

### Method 1: Compile + Link + Run (Step by Step)

```bash
# 1. Compile Apex source to object file
./build/src/apexc/apexc your_program.apex -o output.o

# 2. Link object file to create executable
clang output.o -o output

# 3. Run the executable
./output

# 4. Check exit code
echo "Exit code: $?"
```

### Method 2: One-Liner

```bash
./build/src/apexc/apexc your_program.apex -o /tmp/test.o && \
clang /tmp/test.o -o /tmp/test && \
/tmp/test; echo "Exit code: $?"
```

## Running the Test Suite

```bash
cd tests
./run_tests.sh
```

This will run all 10 test files and verify they produce the correct exit codes.

## Example Programs

### 1. Simple Return Value
```apex
fn main() -> i32 {
    return 42;
}
```
**Expected exit code:** 42

### 2. For Loop (Sum 0..10)
```apex
fn main() -> i32 {
    mut sum = 0;
    for i in 0..10 {
        sum = sum + i;
    }
    return sum;
}
```
**Expected exit code:** 45

### 3. Struct with Fields
```apex
struct Point {
    pub x: i32,
    pub y: i32,
}

fn main() -> i32 {
    let p = Point { x: 3, y: 4 };
    return p.x * p.x + p.y * p.y;  // 25
}
```
**Expected exit code:** 25

## Test Individual Features

All test files are in the `tests/` directory:

- **while_basic.apex** - While loop test
- **for_basic.apex** - For loop test
- **mut_basic.apex** - Mutable variables
- **struct_basic.apex** - Struct types
- **range_basic.apex** - Range expressions

## Current Known Issues

⚠️ **Assignment operator not working:** Statements like `x = new_value` cause compiler crashes. This is a known bug being investigated.

**Workaround:** Use fresh variables or loop constructs that handle mutation internally.

## Compiler Options

```bash
./build/src/apexc/apexc [options] <input.apex>

Options:
  -o <file>       Output file (default: a.out)
  --emit-ast      Print AST and exit
  --emit-llvm     Print LLVM IR (not yet implemented)
  -h, --help      Show help message
```

## Debugging

If compilation fails, check:
1. Syntax errors in your code
2. Type mismatches
3. Undefined variables
4. Missing struct definitions

The compiler will print error messages with line/column numbers.
