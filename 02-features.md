# 2. Language Features

## Core Essentials (C Compatibility)

### Primitive Types

Apex provides explicit-sized integer types and floating-point types with clear semantics.

#### Integer Types

| Type | Description | Size | Range |
|------|-------------|------|-------|
| `i8` | Signed 8-bit integer | 1 byte | -128 to 127 |
| `i16` | Signed 16-bit integer | 2 bytes | -32,768 to 32,767 |
| `i32` | Signed 32-bit integer | 4 bytes | -2³¹ to 2³¹-1 |
| `i64` | Signed 64-bit integer | 8 bytes | -2⁶³ to 2⁶³-1 |
| `i128` | Signed 128-bit integer | 16 bytes | -2¹²⁷ to 2¹²⁷-1 |
| `isize` | Signed pointer-sized integer | Platform | -2ⁿ⁻¹ to 2ⁿ⁻¹-1 |
| `u8` | Unsigned 8-bit integer | 1 byte | 0 to 255 |
| `u16` | Unsigned 16-bit integer | 2 bytes | 0 to 65,535 |
| `u32` | Unsigned 32-bit integer | 4 bytes | 0 to 2³²-1 |
| `u64` | Unsigned 64-bit integer | 8 bytes | 0 to 2⁶⁴-1 |
| `u128` | Unsigned 128-bit integer | 16 bytes | 0 to 2¹²⁸-1 |
| `usize` | Unsigned pointer-sized integer | Platform | 0 to 2ⁿ-1 |

**Example:**
```apex
let x: i32 = -42;
let y: u64 = 1000000;
let ptr_offset: usize = 0x1000;
```

#### Floating-Point Types

| Type | Description | Size | Precision |
|------|-------------|------|-----------|
| `f32` | 32-bit floating point | 4 bytes | ~7 decimal digits |
| `f64` | 64-bit floating point | 8 bytes | ~15 decimal digits |

**Example:**
```apex
let pi: f64 = 3.14159265359;
let height: f32 = 1.75;
```

#### Boolean Type

| Type | Description | Size | Values |
|------|-------------|------|--------|
| `bool` | Boolean value | 1 byte | `true`, `false` |

**Example:**
```apex
let is_valid: bool = true;
let has_error: bool = false;
```

#### Character Types

| Type | Description | Size | Representation |
|------|-------------|------|----------------|
| `char` | Unicode scalar value | 4 bytes | UTF-32 code point |
| `byte` | Raw byte | 1 byte | 0-255 |

**Example:**
```apex
let letter: char = 'A';
let emoji: char = '🚀';
let raw: byte = 0x41;
```

#### Void Type

| Type | Description | Use |
|------|-------------|-----|
| `void` | No value | Function return type only |

### Structs

Structs are composite types that group related data together.

**Basic Struct:**
```apex
struct Point {
    x: i32,
    y: i32,
}

// Usage
let p = Point { x: 10, y: 20 };
let px = p.x;  // Access field
```

**Nested Structs:**
```apex
struct Color {
    r: u8,
    g: u8,
    b: u8,
}

struct Pixel {
    position: Point,
    color: Color,
}

let pixel = Pixel {
    position: Point { x: 100, y: 200 },
    color: Color { r: 255, g: 0, b: 0 },
};
```

**Packed Structs (for hardware/FFI):**
```apex
#[packed]
struct PacketHeader {
    version: u8,
    flags: u8,
    length: u16,
}
```

**Aligned Structs:**
```apex
#[align(16)]
struct Vector4 {
    x: f32,
    y: f32,
    z: f32,
    w: f32,
}
```

### Pointers

Apex distinguishes between safe references and raw pointers.

#### Safe References (Default)

References are checked at compile-time for safety.

```apex
fn add_one(x: &i32) -> i32 {
    return *x + 1;  // Dereference with *
}

let value = 42;
let result = add_one(&value);  // Borrow with &
```

**Mutable References:**
```apex
fn increment(x: &mut i32) {
    *x += 1;
}

let mut count = 0;
increment(&mut count);
```

**Reference Rules:**
- Cannot have mutable and immutable references simultaneously
- References must always point to valid memory
- No null references (use Optional<&T> instead)

#### Raw Pointers (Unsafe)

Raw pointers provide C-like behavior but require `unsafe` blocks.

```apex
unsafe fn manipulate_memory(ptr: *mut u8, size: usize) {
    for i in 0..size {
        *(ptr + i) = 0;  // Pointer arithmetic
    }
}

// Usage
unsafe {
    let ptr = alloc(100) as *mut u8;
    manipulate_memory(ptr, 100);
    dealloc(ptr);
}
```

**Pointer Types:**
- `*T` - Raw immutable pointer
- `*mut T` - Raw mutable pointer
- Can be null, no safety guarantees

### Arrays

Apex provides both fixed-size and dynamic arrays.

#### Fixed-Size Arrays

```apex
// Stack-allocated, size known at compile time
let numbers: [i32; 5] = [1, 2, 3, 4, 5];
let first = numbers[0];
let length = numbers.len;  // Compile-time constant

// Array initialization with default value
let zeros: [i32; 100] = [0; 100];
```

#### Dynamic Arrays (Slices)

```apex
// Reference to a contiguous sequence
fn sum(values: []i32) -> i32 {
    let total = 0;
    for val in values {
        total += val;
    }
    return total;
}

let arr: [i32; 5] = [1, 2, 3, 4, 5];
let result = sum(arr[1..4]);  // Slice: [2, 3, 4]
```

**Slicing Operations:**
```apex
let arr = [0, 1, 2, 3, 4, 5];
let slice1 = arr[1..4];    // [1, 2, 3]
let slice2 = arr[2..];     // [2, 3, 4, 5]
let slice3 = arr[..3];     // [0, 1, 2]
let slice4 = arr[..];      // Full array
```

### Functions

Functions are first-class citizens in Apex.

**Basic Function:**
```apex
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}
```

**Multiple Returns (via tuples):**
```apex
fn divide(a: i32, b: i32) -> (i32, i32) {
    return (a / b, a % b);  // quotient, remainder
}

let (quot, rem) = divide(17, 5);
```

**Function with No Return Value:**
```apex
fn print_message(msg: []char) {
    // No return statement needed for void functions
    io.println(msg);
}
```

**Default Parameters:**
```apex
fn connect(host: []char, port: u16 = 80, timeout: i32 = 30) -> Result<Socket, Error> {
    // Implementation
}

// Can call with defaults
let socket1 = connect("example.com");
let socket2 = connect("example.com", 8080);
let socket3 = connect("example.com", 8080, 60);
```

### Function Pointers

Function pointers enable callbacks and dynamic dispatch.

**Function Pointer Type:**
```apex
// Type: function taking i32, returning i32
type UnaryOp = fn(i32) -> i32;

fn double(x: i32) -> i32 { return x * 2; }
fn square(x: i32) -> i32 { return x * x; }

fn apply(f: UnaryOp, value: i32) -> i32 {
    return f(value);
}

let result1 = apply(double, 5);  // 10
let result2 = apply(square, 5);  // 25
```

**Closures (Capturing Environment):**
```apex
fn make_adder(n: i32) -> fn(i32) -> i32 {
    // Captures 'n' from environment
    return fn(x: i32) -> i32 { return x + n; };
}

let add_five = make_adder(5);
let result = add_five(10);  // 15
```

### Header/Import System (Modules)

Apex replaces C's header system with a proper module system.

**Module Declaration:**
```apex
// File: math/vector.apex
module math.vector;

pub struct Vector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

pub fn dot(a: Vector3, b: Vector3) -> f32 {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

fn helper_function() {
    // Private, not visible outside module
}
```

**Module Import:**
```apex
// Import entire module
import math.vector;
let v = vector.Vector3 { x: 1.0, y: 2.0, z: 3.0 };

// Import specific items
import math.vector.{Vector3, dot};
let v1 = Vector3 { x: 1.0, y: 0.0, z: 0.0 };
let result = dot(v1, v1);

// Import with alias
import math.vector as vec;
let v2 = vec.Vector3 { x: 0.0, y: 1.0, z: 0.0 };
```

**Visibility Modifiers:**
- `pub` - Public, visible outside module
- (default) - Private to module
- `pub(crate)` - Visible within same compilation unit
- `pub(super)` - Visible to parent module

### Compilation Model

Apex uses a module-based compilation model with incremental compilation support.

**Compilation Unit:**
- Each `.apex` file is a compilation unit
- Modules can span multiple files in a directory structure
- Incremental compilation tracks dependencies

**Build Process:**
```apex
// project.apex (build configuration)
project {
    name: "my_app",
    version: "1.0.0",
    
    dependencies: {
        "std": "1.0",
        "network": "2.1",
    },
    
    targets: {
        executable: {
            name: "my_app",
            entry: "main.apex",
        },
    },
}
```

**Separate Compilation:**
- Modules compiled independently
- Type information exported in `.apexmod` files
- Link-time optimization available

### Stack/Heap Allocation

Memory allocation is explicit in Apex.

**Stack Allocation (Default):**
```apex
fn stack_example() {
    let x: i32 = 42;  // Stack allocated
    let arr: [i32; 100] = [0; 100];  // Stack allocated
    let point = Point { x: 10, y: 20 };  // Stack allocated
}  // All automatically cleaned up when function returns
```

**Heap Allocation (Explicit):**
```apex
import std.mem;

fn heap_example() {
    // Allocate single value
    let ptr = mem.alloc<Point>();
    *ptr = Point { x: 10, y: 20 };
    
    // Allocate array
    let arr = mem.alloc_array<i32>(100);
    arr[0] = 42;
    
    // Must explicitly free
    mem.dealloc(ptr);
    mem.dealloc(arr);
}
```

**Smart Pointers (RAII):**
```apex
import std.mem.{Box, Rc, Arc};

fn smart_ptr_example() {
    // Box: Owned heap allocation, auto-freed
    let boxed = Box.new(Point { x: 1, y: 2 });
    
    // Rc: Reference counted (single-threaded)
    let rc1 = Rc.new(42);
    let rc2 = rc1.clone();  // Increment ref count
    
    // Arc: Atomic reference counted (thread-safe)
    let arc = Arc.new(100);
    // Can be shared across threads
}  // All automatically cleaned up
```

### Manual Memory Management

Apex provides low-level control when needed.

**Custom Allocators:**
```apex
struct Arena {
    buffer: *mut u8,
    size: usize,
    used: usize,
}

impl Arena {
    fn alloc(&mut self, size: usize) -> *mut u8 {
        if self.used + size > self.size {
            return null;
        }
        let ptr = self.buffer + self.used;
        self.used += size;
        return ptr;
    }
    
    fn reset(&mut self) {
        self.used = 0;
    }
}
```

**Memory Management Strategies:**
- Stack allocation for short-lived data
- Heap allocation for dynamic size or lifetime
- Custom allocators for performance-critical code
- Pool allocators for fixed-size objects
- Arena allocators for bulk deallocation

### Undefined Behavior Policy

Apex eliminates most undefined behavior from C.

**Safe Mode (Default):**
- Array bounds checked
- Integer overflow checked (panics or wraps based on mode)
- No null pointer dereferences
- No use-after-free
- No data races
- Alignment guaranteed

**Checked Operations:**
```apex
fn safe_operations() {
    let arr: [i32; 5] = [1, 2, 3, 4, 5];
    let value = arr[10];  // Compile error or runtime panic
    
    let x: i32 = 2000000000;
    let y = x + x;  // Overflow detected, panic (or wrap if configured)
}
```

**Unsafe Mode (Explicit):**
```apex
unsafe fn unchecked_access(arr: []i32, index: usize) -> i32 {
    // No bounds checking, programmer responsibility
    return arr.unchecked_get(index);
}

unsafe {
    let arr = [1, 2, 3];
    let val = unchecked_access(arr, 1);  // OK
    // let bad = unchecked_access(arr, 10);  // Undefined behavior!
}
```

**Allowed in Unsafe Blocks:**
- Raw pointer arithmetic
- Unchecked array access
- Type punning
- Inline assembly
- Calling unsafe functions
- Accessing mutable statics

### Bitwise Operations

Full set of bitwise operations with clear precedence.

**Bitwise Operators:**
```apex
let a: u32 = 0b11110000;
let b: u32 = 0b10101010;

let and = a & b;      // Bitwise AND:  10100000
let or  = a | b;      // Bitwise OR:   11111010
let xor = a ^ b;      // Bitwise XOR:  01011010
let not = ~a;         // Bitwise NOT:  00001111

let shl = a << 2;     // Shift left:   11000000 00
let shr = a >> 2;     // Shift right:  00111100
```

**Bit Manipulation:**
```apex
fn set_bit(value: u32, bit: u32) -> u32 {
    return value | (1 << bit);
}

fn clear_bit(value: u32, bit: u32) -> u32 {
    return value & ~(1 << bit);
}

fn toggle_bit(value: u32, bit: u32) -> u32 {
    return value ^ (1 << bit);
}

fn test_bit(value: u32, bit: u32) -> bool {
    return (value & (1 << bit)) != 0;
}
```

### Low-Level Access

Apex provides direct access to hardware and memory.

**Register Access:**
```apex
unsafe fn read_timer_register() -> u32 {
    let timer_base: usize = 0x40000000;
    let timer_ptr = timer_base as *const u32;
    return *timer_ptr;
}

unsafe fn write_gpio(pin: u8, value: bool) {
    let gpio_base: usize = 0x50000000;
    let gpio_ptr = gpio_base as *mut u32;
    if value {
        *gpio_ptr |= (1 << pin);
    } else {
        *gpio_ptr &= ~(1 << pin);
    }
}
```

**Memory-Mapped I/O:**
```apex
#[repr(C)]
#[volatile]
struct UARTRegisters {
    data: u32,
    status: u32,
    control: u32,
}

unsafe fn uart_send(ch: char) {
    let uart = 0x40010000 as *mut UARTRegisters;
    while ((*uart).status & 0x80) == 0 {
        // Wait for transmit ready
    }
    (*uart).data = ch as u32;
}
```

**Inline Assembly:**
```apex
unsafe fn disable_interrupts() {
    asm!("cli");
}

unsafe fn read_msr(msr: u32) -> u64 {
    let low: u32;
    let high: u32;
    asm!(
        "rdmsr",
        in("ecx") msr,
        out("eax") low,
        out("edx") high,
    );
    return ((high as u64) << 32) | (low as u64);
}
```

## Modern Enhancements

### Modules Instead of Header Files

No more `#include` guards or header duplication.

**Traditional C Problem:**
```c
// header.h
#ifndef HEADER_H
#define HEADER_H
void function();
#endif

// source.c
#include "header.h"
void function() { /* ... */ }
```

**Apex Solution:**
```apex
// math.apex
module math;

pub fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

// main.apex
import math;

fn main() {
    let result = math.add(5, 3);
}
```

### Improved Type System

**Type Inference:**
```apex
let x = 42;        // Inferred as i32
let y = 3.14;      // Inferred as f64
let s = "hello";   // Inferred as string
```

**Strong Type Safety:**
```apex
let x: i32 = 42;
let y: i64 = x;     // Error: must explicitly cast
let y: i64 = x as i64;  // OK
```

**Optional Types (No Null):**
```apex
fn find_user(id: i32) -> Optional<User> {
    if user_exists(id) {
        return Some(get_user(id));
    } else {
        return None;
    }
}

let user = find_user(42);
match user {
    Some(u) => io.println("Found: {}", u.name),
    None => io.println("Not found"),
}
```

**Result Types:**
```apex
fn divide(a: i32, b: i32) -> Result<i32, Error> {
    if b == 0 {
        return Err(Error.new("Division by zero"));
    }
    return Ok(a / b);
}

let result = divide(10, 2);
match result {
    Ok(value) => io.println("Result: {}", value),
    Err(e) => io.println("Error: {}", e.message),
}
```

### Generics

True generics with compile-time monomorphization (no runtime cost).

**Generic Functions:**
```apex
fn max<T>(a: T, b: T) -> T where T: Comparable {
    if a > b {
        return a;
    } else {
        return b;
    }
}

let int_max = max(10, 20);      // Monomorphized for i32
let float_max = max(3.14, 2.71); // Monomorphized for f64
```

**Generic Structs:**
```apex
struct Vec<T> {
    data: *mut T,
    length: usize,
    capacity: usize,
}

impl<T> Vec<T> {
    fn new() -> Vec<T> {
        return Vec {
            data: null,
            length: 0,
            capacity: 0,
        };
    }
    
    fn push(&mut self, value: T) {
        // Implementation
    }
    
    fn get(&self, index: usize) -> Optional<&T> {
        if index < self.length {
            return Some(&self.data[index]);
        }
        return None;
    }
}
```

**Type Constraints:**
```apex
trait Numeric {
    fn add(self, other: Self) -> Self;
    fn sub(self, other: Self) -> Self;
    fn mul(self, other: Self) -> Self;
    fn div(self, other: Self) -> Self;
}

fn sum<T>(values: []T) -> T where T: Numeric + Default {
    let total = T.default();
    for val in values {
        total = total.add(val);
    }
    return total;
}
```

### Pattern Matching

Exhaustive pattern matching for control flow.

**Match Expression:**
```apex
enum Result<T, E> {
    Ok(T),
    Err(E),
}

fn handle_result(res: Result<i32, string>) -> i32 {
    match res {
        Ok(value) => return value,
        Err(msg) => {
            io.println("Error: {}", msg);
            return -1;
        },
    }
}
```

**Destructuring:**
```apex
struct Point { x: i32, y: i32 }

fn classify(p: Point) -> string {
    match p {
        Point { x: 0, y: 0 } => return "origin",
        Point { x: 0, y: _ } => return "on y-axis",
        Point { x: _, y: 0 } => return "on x-axis",
        Point { x, y } if x == y => return "diagonal",
        _ => return "general",
    }
}
```

**If-Let Syntax:**
```apex
let optional_value: Optional<i32> = Some(42);

if let Some(value) = optional_value {
    io.println("Value: {}", value);
}
```

### Memory Safety Modes

**Safe Mode (Default):**
```apex
fn safe_function() {
    let arr = [1, 2, 3, 4, 5];
    let value = arr[2];  // Bounds checked
    
    let x: i32 = 1000;
    let y = x * 1000;    // Overflow checked
}
```

**Unsafe Blocks:**
```apex
fn use_unsafe_code() {
    let data = [1, 2, 3, 4, 5];
    
    unsafe {
        let ptr = data.as_ptr();
        let value = *(ptr + 2);  // No bounds check
        
        // Can call unsafe functions
        raw_memory_copy(ptr, dest_ptr, 5);
    }
    
    // Back to safe mode here
}
```

**Unsafe Functions:**
```apex
unsafe fn raw_memory_copy(src: *const u8, dst: *mut u8, count: usize) {
    for i in 0..count {
        *(dst + i) = *(src + i);
    }
}
```

### Built-in Concurrency Model

**Thread Creation:**
```apex
import std.thread;

fn worker(id: i32) {
    io.println("Worker {} started", id);
    thread.sleep_ms(1000);
    io.println("Worker {} finished", id);
}

fn main() {
    let threads = Vec.new<Thread>();
    
    for i in 0..4 {
        let t = thread.spawn(|| worker(i));
        threads.push(t);
    }
    
    for t in threads {
        t.join();
    }
}
```

**Channels (Message Passing):**
```apex
import std.sync.Channel;

fn main() {
    let (tx, rx) = Channel.new<i32>();
    
    thread.spawn(|| {
        for i in 0..10 {
            tx.send(i);
        }
    });
    
    for value in rx {
        io.println("Received: {}", value);
    }
}
```

**Mutexes:**
```apex
import std.sync.Mutex;

let counter = Mutex.new(0);

fn increment() {
    let mut guard = counter.lock();
    *guard += 1;
}  // Lock released when guard goes out of scope
```

### Error-Handling Model

**Result Type (Primary Method):**
```apex
enum Result<T, E> {
    Ok(T),
    Err(E),
}

fn read_file(path: string) -> Result<string, IoError> {
    let file = File.open(path)?;  // Propagate error with ?
    let contents = file.read_to_string()?;
    return Ok(contents);
}
```

**Try-Catch (for Panics):**
```apex
fn risky_operation() -> i32 {
    try {
        let result = may_panic();
        return result * 2;
    } catch (e: PanicError) {
        io.println("Caught panic: {}", e.message);
        return -1;
    }
}
```

**Assertions:**
```apex
fn compute(x: i32) -> i32 {
    assert(x > 0, "x must be positive");
    assert(x < 1000);  // Simple assertion
    
    debug_assert(x % 2 == 0);  // Only in debug builds
    
    return x * 2;
}
```

### Standard Library Design Goals

1. **Minimal Core**: Small core that compiles fast
2. **Modular**: Import only what you need
3. **Zero-Cost**: Abstractions compile to optimal code
4. **Portable**: Works across platforms
5. **Safe Defaults**: Unsafe operations explicitly marked
6. **Comprehensive**: Everything needed for systems programming

**Standard Library Modules:**
- `std.io` - Input/output operations
- `std.mem` - Memory management
- `std.thread` - Threading primitives
- `std.sync` - Synchronization primitives
- `std.fs` - File system operations
- `std.net` - Networking
- `std.math` - Mathematical functions
- `std.time` - Time and date handling
- `std.collections` - Data structures
- `std.string` - String manipulation

### Modern Syntax Improvements

**String Interpolation:**
```apex
let name = "Alice";
let age = 30;
io.println("Hello, {}! You are {} years old.", name, age);
```

**Range Expressions:**
```apex
for i in 0..10 {        // 0 to 9
    io.println(i);
}

for i in 0..=10 {       // 0 to 10 (inclusive)
    io.println(i);
}
```

**Defer Statements:**
```apex
fn process_file(path: string) {
    let file = File.open(path).unwrap();
    defer file.close();  // Executed when function exits
    
    // File operations...
    // file.close() called automatically
}
```

**Multiple Assignment:**
```apex
let (x, y, z) = (1, 2, 3);

let (first, rest) = split_first([1, 2, 3, 4]);
// first = 1, rest = [2, 3, 4]
```

**Method Syntax:**
```apex
impl Point {
    fn distance(&self, other: Point) -> f64 {
        let dx = (self.x - other.x) as f64;
        let dy = (self.y - other.y) as f64;
        return math.sqrt(dx * dx + dy * dy);
    }
}

let p1 = Point { x: 0, y: 0 };
let p2 = Point { x: 3, y: 4 };
let dist = p1.distance(p2);  // Method call syntax
```

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  

**Next**: [03-syntax.md](03-syntax.md) - Complete Syntax Examples
