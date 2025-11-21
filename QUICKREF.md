# Apex Language Quick Reference

## Basic Syntax

### Variables
```apex
let x: i32 = 42;              // Immutable
let mut y: i32 = 10;          // Mutable
const PI: f64 = 3.14159;      // Compile-time constant
```

### Functions
```apex
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn main() -> i32 {
    return add(5, 3);
}
```

### Control Flow
```apex
// If expression
let result = if x > 0 {
    1
} else {
    -1
};

// Match expression
match value {
    0 => io::println("zero"),
    1 => io::println("one"),
    _ => io::println("other"),
}

// Loops
while x < 10 {
    x += 1;
}

for i in 0..10 {
    io::println(i);
}
```

### Data Types
```apex
// Primitives
let a: i32 = 42;        // 32-bit signed integer
let b: u64 = 100;       // 64-bit unsigned integer
let c: f32 = 3.14;      // 32-bit float
let d: bool = true;     // Boolean
let e: char = 'A';      // Character

// Structs
struct Point {
    pub x: i32,
    pub y: i32,
}

let p = Point { x: 10, y: 20 };

// Enums
enum Color {
    Red,
    Green,
    Blue,
}

// Arrays
let arr: [i32; 5] = [1, 2, 3, 4, 5];
let repeated: [i32; 10] = [0; 10];

// Tuples
let tuple: (i32, f64, bool) = (42, 3.14, true);
```

### Pointers and References
```apex
// Safe references
let x: i32 = 42;
let ref_x: &i32 = &x;           // Immutable reference
let mut y: i32 = 10;
let ref_mut_y: &mut i32 = &mut y;  // Mutable reference

// Raw pointers (unsafe)
unsafe {
    let ptr: *i32 = &x as *i32;
    let val = *ptr;
}
```

### Modules
```apex
// Define module
mod math {
    pub fn square(x: i32) -> i32 {
        return x * x;
    }
}

// Import module
import std::io;
import math::square;

fn main() {
    io::println("Hello");
    let result = square(5);
}
```

### Generics
```apex
struct Box<T> {
    value: T,
}

fn max<T>(a: T, b: T) -> T {
    if a > b { a } else { b }
}
```

### Traits
```apex
trait Printable {
    fn print(&self);
}

impl Printable for Point {
    fn print(&self) {
        io::println("Point({}, {})", self.x, self.y);
    }
}
```

## Operators

### Arithmetic
```apex
+  -  *  /  %       // Add, subtract, multiply, divide, modulo
+= -= *= /= %=      // Compound assignment
```

### Comparison
```apex
== != < <= > >=     // Equality, less than, greater than
```

### Logical
```apex
&& || !             // AND, OR, NOT
```

### Bitwise
```apex
& | ^ ~ << >>       // AND, OR, XOR, NOT, shift left, shift right
&= |= ^= <<= >>=    // Compound bitwise
```

### Other
```apex
= -> => :: . .. ..= ? @ &  // Assignment, arrow, fat arrow, scope, dot, range, etc.
```

## Primitive Types

| Type | Description | Size | Range |
|------|-------------|------|-------|
| `i8` | Signed integer | 8-bit | -128 to 127 |
| `i16` | Signed integer | 16-bit | -32,768 to 32,767 |
| `i32` | Signed integer | 32-bit | -2³¹ to 2³¹-1 |
| `i64` | Signed integer | 64-bit | -2⁶³ to 2⁶³-1 |
| `i128` | Signed integer | 128-bit | -2¹²⁷ to 2¹²⁷-1 |
| `isize` | Pointer-sized signed | Platform | Depends on platform |
| `u8` | Unsigned integer | 8-bit | 0 to 255 |
| `u16` | Unsigned integer | 16-bit | 0 to 65,535 |
| `u32` | Unsigned integer | 32-bit | 0 to 2³²-1 |
| `u64` | Unsigned integer | 64-bit | 0 to 2⁶⁴-1 |
| `u128` | Unsigned integer | 128-bit | 0 to 2¹²⁸-1 |
| `usize` | Pointer-sized unsigned | Platform | Depends on platform |
| `f32` | Floating point | 32-bit | IEEE 754 single |
| `f64` | Floating point | 64-bit | IEEE 754 double |
| `bool` | Boolean | 1-bit | true or false |
| `char` | Unicode character | 32-bit | Any Unicode scalar |

## Compiler Commands

### Basic Compilation
```bash
apexc program.apx              # Compile to program.o
apexc -o output.o program.apx  # Specify output file
```

### Debug Modes
```bash
apexc --emit-tokens program.apx   # Show tokens
apexc --emit-ast program.apx      # Show AST
apexc --emit-llvm program.apx     # Generate LLVM IR
apexc -v program.apx              # Verbose output
```

### Help
```bash
apexc --help                    # Show help message
```

## Standard Library (Planned)

```apex
import std::io;         // Input/output
import std::fs;         // File system
import std::mem;        // Memory (Box, Rc, Arc)
import std::collections;// Vec, HashMap, HashSet
import std::thread;     // Threading
import std::sync;       // Synchronization
import std::net;        // Networking
import std::time;       // Time and date
```

## Memory Safety

### Ownership Rules
1. Each value has exactly one owner
2. When owner goes out of scope, value is freed
3. Values can be borrowed (referenced) but not owned by multiple places

### Borrowing Rules
1. Any number of immutable references OR one mutable reference
2. References must always be valid
3. No dangling pointers

### Safe vs Unsafe
```apex
// Safe by default
fn safe_code() {
    let x: i32 = 42;
    let y = &x;  // Compiler checks validity
}

// Explicit unsafe blocks
fn unsafe_code() {
    unsafe {
        let ptr: *i32 = 0x1000 as *i32;
        let val = *ptr;  // Your responsibility
    }
}
```

## Common Patterns

### Result Type
```apex
enum Result<T, E> {
    Ok(T),
    Err(E),
}

fn divide(a: i32, b: i32) -> Result<i32, &str> {
    if b == 0 {
        return Result::Err("division by zero");
    }
    return Result::Ok(a / b);
}
```

### Option Type
```apex
enum Option<T> {
    Some(T),
    None,
}

fn find(arr: &[i32], value: i32) -> Option<usize> {
    for i in 0..arr.len() {
        if arr[i] == value {
            return Option::Some(i);
        }
    }
    return Option::None;
}
```

### Defer
```apex
fn process_file(path: &str) {
    let file = open(path);
    defer close(file);  // Runs at end of scope
    
    // Use file...
    // file automatically closed
}
```

## Resources

- **Specification:** See files 01-10 in repository
- **Examples:** `examples/` directory
- **Build Guide:** `docs/BUILD.md`
- **Contributing:** `CONTRIBUTING.md`
- **Status:** `STATUS.md`

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  

For complete documentation, see the full language specification files.
