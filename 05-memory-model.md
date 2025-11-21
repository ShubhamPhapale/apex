# 5. Memory Model and Safety Rules

## Overview

Apex's memory model is designed to provide C-like control with compile-time safety guarantees. The language uses a combination of static analysis, ownership tracking, and explicit unsafe blocks to prevent common memory errors while maintaining zero-cost abstractions.

## Stack vs Heap Semantics

### Stack Allocation

Stack allocation is the default and preferred method for local variables with known size at compile time.

**Characteristics:**
- Automatic allocation and deallocation
- LIFO (Last In, First Out) order
- Fast allocation (just pointer adjustment)
- Limited size (typically 1-8 MB)
- Deterministic lifetime (lexical scope)

**Stack Allocated Types:**
```apex
fn stack_example() {
    // All stack allocated
    let x: i32 = 42;                          // 4 bytes
    let arr: [i32; 100] = [0; 100];          // 400 bytes
    let point = Point { x: 10, y: 20 };       // 8 bytes
    let nested = (1, 2.5, "hello");           // Variable size
}  // All memory automatically freed here
```

**Stack Growth:**
- Stack grows downward (on most architectures)
- Each function call creates a new stack frame
- Stack frame contains: local variables, parameters, return address

**Stack Overflow:**
- Detected at runtime when stack limit exceeded
- Can be configured via compiler flags or runtime settings
- Deep recursion or large local arrays can cause overflow

### Heap Allocation

Heap allocation is explicit and used for:
- Dynamically sized data
- Data that outlives function scope
- Large data structures
- Shared data across threads

**Explicit Heap Allocation:**
```apex
import std.mem;

fn heap_example() {
    // Manual allocation
    let ptr: *mut i32 = mem.alloc<i32>();
    unsafe {
        *ptr = 42;
        io.println("Value: {}", *ptr);
        mem.dealloc(ptr);  // Must manually free
    }
    
    // Array allocation
    let arr: *mut i32 = mem.alloc_array<i32>(100);
    unsafe {
        arr[0] = 1;
        mem.dealloc(arr);
    }
}
```

**Smart Pointers (RAII):**
```apex
import std.mem.{Box, Rc, Arc};

fn smart_pointers() {
    // Box: Single ownership, auto-freed
    let boxed = Box.new(Point { x: 1, y: 2 });
    // Automatically freed when boxed goes out of scope
    
    // Rc: Reference counted (single-threaded)
    let rc1 = Rc.new(42);
    let rc2 = rc1.clone();  // Ref count = 2
    // Freed when last reference dropped
    
    // Arc: Atomic reference counted (thread-safe)
    let arc = Arc.new(100);
    // Can be safely shared across threads
}
```

**Heap Fragmentation:**
- Long-running programs may experience fragmentation
- Use custom allocators for specific access patterns
- Arena allocators for bulk deallocation
- Pool allocators for fixed-size objects

## Ownership and Borrowing

Apex uses an ownership system similar to Rust but with more explicit syntax for clarity.

### Ownership Rules

1. Each value has a single owner
2. When the owner goes out of scope, the value is dropped
3. Ownership can be transferred (moved)
4. Values can be borrowed (referenced) without transferring ownership

### Move Semantics

```apex
fn move_example() {
    let s1 = String.from("hello");
    let s2 = s1;  // s1 moved to s2, s1 no longer valid
    
    // io.println("{}", s1);  // ERROR: s1 moved
    io.println("{}", s2);     // OK
}
```

**Copy Types:**
Types that implement the `Copy` trait are copied instead of moved:
- All primitive types (i32, f64, bool, char, etc.)
- Fixed-size arrays of Copy types
- Tuples of Copy types
- Types explicitly marked with `#[derive(Copy)]`

```apex
fn copy_example() {
    let x = 42;
    let y = x;  // x is copied, both valid
    
    io.println("{}, {}", x, y);  // OK: both valid
}
```

### References and Borrowing

**Immutable References:**
```apex
fn immutable_borrow() {
    let s = String.from("hello");
    let r1 = &s;  // Immutable borrow
    let r2 = &s;  // Multiple immutable borrows OK
    
    io.println("{} {}", r1, r2);
    io.println("{}", s);  // Original still accessible
}
```

**Mutable References:**
```apex
fn mutable_borrow() {
    let mut s = String.from("hello");
    let r = &mut s;  // Mutable borrow
    
    r.push_str(" world");
    // io.println("{}", s);  // ERROR: s borrowed mutably
    io.println("{}", r);     // OK
}  // r goes out of scope, s accessible again
```

**Borrowing Rules:**
1. At any time, you can have either:
   - One mutable reference, OR
   - Any number of immutable references
2. References must always be valid
3. No null references (use Optional<&T> instead)

### Lifetime Annotations

Lifetimes ensure references are valid for their entire use.

**Explicit Lifetimes:**
```apex
fn longest<'a>(x: &'a string, y: &'a string) -> &'a string {
    if x.len() > y.len() {
        return x;
    } else {
        return y;
    }
}
```

**Lifetime Elision Rules:**
1. Each reference parameter gets its own lifetime
2. If there's one input lifetime, it's assigned to all output lifetimes
3. If there's a `&self` or `&mut self` parameter, its lifetime is assigned to all output lifetimes

**Common Lifetime Patterns:**
```apex
// Static lifetime - lives for entire program
fn get_string() -> &'static string {
    return "Hello";
}

// Struct with lifetime
struct Reference<'a> {
    data: &'a string,
}

impl<'a> Reference<'a> {
    fn get_data(&self) -> &'a string {
        return self.data;
    }
}
```

## Pointer Safety Rules

### Safe References (Default)

**Compile-Time Guarantees:**
- Never null
- Always point to valid memory
- Cannot dangle (outlive referenced data)
- No data races
- Bounds-checked array access through references

**Type System Enforcement:**
```apex
fn safe_references() {
    let x = 42;
    let r = &x;
    
    // All these are caught at compile time:
    // let null_ref: &i32 = null;  // ERROR: null not allowed
    // let dangling = get_dangling_ref();  // ERROR: lifetime error
    // *r = 10;  // ERROR: cannot mutate through immutable reference
}
```

### Raw Pointers (Unsafe)

Raw pointers provide C-like flexibility but require `unsafe` blocks.

**Raw Pointer Types:**
- `*const T` - Immutable raw pointer
- `*mut T` - Mutable raw pointer

**Raw Pointer Capabilities:**
```apex
unsafe fn raw_pointer_operations() {
    let x = 42;
    
    // Creating raw pointers is safe
    let ptr_const: *const i32 = &x;
    let ptr_mut: *mut i32 = &x as *const i32 as *mut i32;
    
    // But dereferencing requires unsafe
    let value = *ptr_const;
    *ptr_mut = 100;
    
    // Pointer arithmetic
    let arr = [1, 2, 3, 4, 5];
    let ptr = arr.as_ptr();
    let second = *(ptr + 1);  // 2
    
    // Null pointers allowed
    let null_ptr: *const i32 = null;
    if null_ptr != null {
        let value = *null_ptr;
    }
}
```

**When to Use Raw Pointers:**
- FFI with C code
- Implementing low-level data structures
- Interfacing with hardware
- Performance-critical code with manual bounds checking
- Type punning and reinterpretation

## Memory Safety Modes

### Safe Mode (Default)

All code is safe by default with compile-time and runtime checks.

**Safety Guarantees:**
```apex
fn safe_mode() {
    let arr = [1, 2, 3, 4, 5];
    
    // Bounds checking (compile-time if constant, runtime otherwise)
    let valid = arr[2];     // OK
    // let invalid = arr[10]; // Runtime panic
    
    // Integer overflow checking
    let x: i32 = 2_000_000_000;
    // let y = x + x;  // Runtime panic (or wrap with -Zoverflow-checks=off)
    
    // No null dereferences
    let opt: Optional<i32> = Some(42);
    match opt {
        Some(v) => io.println("{}", v),
        None => io.println("No value"),
    }
}
```

**Checked Operations:**
- Array bounds
- Integer overflow/underflow
- Division by zero
- Pointer validity
- Type safety
- Data race freedom

### Unsafe Blocks

Unsafe blocks allow operations that bypass safety checks.

**Unsafe Capabilities:**
```apex
unsafe fn unsafe_operations() {
    // 1. Dereference raw pointers
    let ptr: *mut i32 = mem.alloc<i32>();
    *ptr = 42;
    
    // 2. Call unsafe functions
    raw_memory_copy(src, dst, size);
    
    // 3. Access/modify mutable statics
    GLOBAL_COUNTER += 1;
    
    // 4. Implement unsafe traits
    impl UnsafeTrait for MyType { }
    
    // 5. Access union fields
    let u: MyUnion = MyUnion { int_val: 42 };
    let float_view = u.float_val;
    
    // 6. Inline assembly
    asm!("nop");
}
```

**Unsafe Function Declaration:**
```apex
unsafe fn dangerous_operation(ptr: *mut u8, len: usize) {
    for i in 0..len {
        *(ptr + i) = 0;
    }
}

// Must call in unsafe block or unsafe function
unsafe {
    dangerous_operation(some_ptr, 100);
}
```

### Gradual Safety

Apex allows mixing safe and unsafe code freely with clear boundaries.

**Pattern: Safe Interface, Unsafe Implementation:**
```apex
pub struct Vec<T> {
    ptr: *mut T,
    len: usize,
    capacity: usize,
}

impl<T> Vec<T> {
    // Safe public API
    pub fn push(&mut self, value: T) {
        if self.len == self.capacity {
            self.grow();
        }
        unsafe {
            // Unsafe implementation
            *(self.ptr + self.len) = value;
        }
        self.len += 1;
    }
    
    pub fn get(&self, index: usize) -> Optional<&T> {
        if index < self.len {
            unsafe {
                return Some(&*(self.ptr + index));
            }
        }
        return None;
    }
    
    // Unsafe implementation details
    unsafe fn grow(&mut self) {
        let new_capacity = if self.capacity == 0 { 4 } else { self.capacity * 2 };
        let new_ptr = mem.alloc_array<T>(new_capacity);
        
        if self.ptr != null {
            mem.copy(self.ptr, new_ptr, self.len);
            mem.dealloc(self.ptr);
        }
        
        self.ptr = new_ptr;
        self.capacity = new_capacity;
    }
}
```

## Memory Layout and Alignment

### Type Sizes and Alignment

**Primitive Type Sizes:**
```apex
size_of::<i8>()    == 1,  align_of::<i8>()    == 1
size_of::<i16>()   == 2,  align_of::<i16>()   == 2
size_of::<i32>()   == 4,  align_of::<i32>()   == 4
size_of::<i64>()   == 8,  align_of::<i64>()   == 8
size_of::<i128>()  == 16, align_of::<i128>()  == 16
size_of::<f32>()   == 4,  align_of::<f32>()   == 4
size_of::<f64>()   == 8,  align_of::<f64>()   == 8
size_of::<bool>()  == 1,  align_of::<bool>()  == 1
size_of::<char>()  == 4,  align_of::<char>()  == 4
size_of::<*T>()    == 8,  align_of::<*T>()    == 8  // On 64-bit
```

### Struct Layout

**Default Layout (Optimized):**
```apex
struct Example {
    a: u8,   // offset 0
    b: u32,  // offset 4 (padding 3 bytes)
    c: u16,  // offset 8
    d: u8,   // offset 10
    // Total: 11 bytes + 5 padding = 16 bytes (aligned to 4)
}

size_of::<Example>() == 16
align_of::<Example>() == 4
```

**Repr(C) Layout (C-compatible):**
```apex
#[repr(C)]
struct CExample {
    a: u8,   // offset 0
    b: u32,  // offset 4 (padding 3 bytes)
    c: u16,  // offset 8
    d: u8,   // offset 10
    // Total: 12 bytes (C layout, may differ from Apex default)
}
```

**Packed Layout (No padding):**
```apex
#[repr(packed)]
struct Packed {
    a: u8,   // offset 0
    b: u32,  // offset 1 (no alignment)
    c: u16,  // offset 5
    d: u8,   // offset 7
    // Total: 8 bytes, no padding
}

// Warning: Accessing fields may be slower due to misalignment
```

**Aligned Layout (Explicit alignment):**
```apex
#[repr(align(16))]
struct Aligned {
    data: [u8; 12],
}

align_of::<Aligned>() == 16
```

### Array and Slice Layout

**Fixed-Size Arrays:**
```apex
let arr: [i32; 5] = [1, 2, 3, 4, 5];
// Memory: [1][2][3][4][5] - contiguous, no overhead
// size_of::<[i32; 5]>() == 20
```

**Slices (Fat Pointers):**
```apex
let slice: []i32 = arr[1..4];
// Slice is a fat pointer:
// - pointer to first element (8 bytes)
// - length (8 bytes)
// Total: 16 bytes on 64-bit systems
```

### Enum Layout

**Simple Enum (Tag only):**
```apex
enum Color {
    Red,    // discriminant: 0
    Green,  // discriminant: 1
    Blue,   // discriminant: 2
}
// size_of::<Color>() == 1 (smallest integer to represent 3 values)
```

**Enum with Data (Tagged Union):**
```apex
enum Message {
    Quit,                           // Tag: 0
    Move { x: i32, y: i32 },       // Tag: 1
    Write(string),                  // Tag: 2
    ChangeColor(u8, u8, u8),       // Tag: 3
}

// Layout: Tag (discriminant) + largest variant data
// size_of::<Message>() == tag_size + max(variant_sizes) + alignment padding
```

**Optimized Enum Layout:**
```apex
enum Optional<T> {
    Some(T),
    None,
}

// Compiler optimizations:
// - For references: use null pointer as None (0 overhead)
// - For non-zero integers: use 0 as None
// - Otherwise: explicit tag
```

## Thread Safety and Data Races

### Send and Sync Traits

Apex uses marker traits to enforce thread safety at compile time.

**Send Trait:**
Types that can be transferred across thread boundaries.
```apex
trait Send { }

// Automatically implemented for types that are safe to send:
// - Primitives (i32, f64, etc.)
// - Owned pointers (Box<T> where T: Send)
// - Most user types

// Not Send:
// - Raw pointers (*const T, *mut T)
// - Rc<T> (reference counted, not thread-safe)
// - Types containing !Send types
```

**Sync Trait:**
Types that can be safely shared across threads (thread-safe references).
```apex
trait Sync { }

// T is Sync if &T is Send
// Automatically implemented for types where shared access is safe:
// - Primitives
// - Immutable types
// - Types protected by synchronization primitives

// Not Sync:
// - Cell<T>, RefCell<T> (interior mutability without synchronization)
// - Raw pointers
// - Types containing !Sync types
```

**Thread-Safe Types:**
```apex
import std.sync.{Arc, Mutex, RwLock};

fn thread_safe_example() {
    // Arc<T>: Send + Sync if T: Send + Sync
    let shared_data = Arc.new(Mutex.new(vec![1, 2, 3]));
    
    let data_clone = shared_data.clone();
    thread.spawn(|| {
        let mut data = data_clone.lock().unwrap();
        data.push(4);
    });
}
```

### Data Race Prevention

**Compile-Time Checks:**
```apex
fn no_data_races() {
    let mut counter = 0;
    
    // This won't compile - trying to share mutable state
    /*
    thread.spawn(|| {
        counter += 1;  // ERROR: cannot capture mutable variable
    });
    */
    
    // Correct: use atomic or mutex
    let counter = Arc.new(Mutex.new(0));
    let counter_clone = counter.clone();
    
    thread.spawn(|| {
        let mut guard = counter_clone.lock().unwrap();
        *guard += 1;  // OK: protected by mutex
    });
}
```

**Atomic Operations:**
```apex
import std.sync.atomic.{AtomicI32, AtomicBool, Ordering};

static COUNTER: AtomicI32 = AtomicI32.new(0);

fn atomic_operations() {
    // Load
    let value = COUNTER.load(Ordering.Acquire);
    
    // Store
    COUNTER.store(42, Ordering.Release);
    
    // Fetch and modify
    let old = COUNTER.fetch_add(1, Ordering.SeqCst);
    
    // Compare and swap
    let result = COUNTER.compare_exchange(
        old, new, Ordering.SeqCst, Ordering.Relaxed
    );
}
```

### Memory Ordering

Apex provides memory ordering semantics for atomic operations:

| Ordering | Description | Use Case |
|----------|-------------|----------|
| `Relaxed` | No ordering constraints | Independent atomic ops |
| `Acquire` | Prevents reordering of loads | Lock acquisition |
| `Release` | Prevents reordering of stores | Lock release |
| `AcqRel` | Both Acquire and Release | Read-modify-write |
| `SeqCst` | Sequential consistency | Safest, most restrictive |

## Interaction with OS Memory

### Virtual Memory

**Address Space:**
- Each process has its own virtual address space
- Typical layout (64-bit Linux):
  - Stack: High addresses (grows down)
  - Memory-mapped region: Middle
  - Heap: Low addresses (grows up)
  - Code/Data: Lowest addresses

**Memory Pages:**
- OS allocates memory in pages (typically 4 KB)
- `mem.alloc()` may request pages from OS
- Pages can be: readable, writable, executable
- Guard pages detect stack overflow

### System Calls

**Memory Allocation System Calls:**
```apex
// These wrap system calls like mmap, munmap, brk, sbrk
import std.sys.memory;

unsafe fn low_level_allocation() {
    // Request memory from OS
    let ptr = memory.mmap(
        addr: null,
        length: 4096,
        prot: memory.PROT_READ | memory.PROT_WRITE,
        flags: memory.MAP_PRIVATE | memory.MAP_ANONYMOUS,
    );
    
    if ptr == memory.MAP_FAILED {
        panic("Failed to allocate memory");
    }
    
    // Use memory...
    
    // Return to OS
    memory.munmap(ptr, 4096);
}
```

### Memory-Mapped I/O

```apex
import std.fs.File;

fn memory_mapped_file() -> Result<void, Error> {
    let file = File.open("large_file.dat")?;
    let mapping = file.mmap()?;
    
    // Access file contents as memory
    let data = mapping.as_slice();
    let first_byte = data[0];
    
    // Modifications written back to file
    let mut_mapping = file.mmap_mut()?;
    mut_mapping.as_slice_mut()[0] = 42;
    
    return Ok(void);
}
```

## Zero-Sized Types (ZSTs)

Types with zero size are optimized away by the compiler.

```apex
struct Empty { }
struct Unit;

size_of::<Empty>() == 0
size_of::<Unit>() == 0
size_of::<()>() == 0  // Unit type

// Arrays of ZSTs
let arr: [Unit; 1000000] = [Unit; 1000000];
size_of::<[Unit; 1000000]>() == 0  // No allocation!
```

**Use Cases:**
- Marker types for type-level programming
- State machines with zero-cost states
- Phantom data for generic constraints

## Summary

**Key Guarantees:**
1. Memory safety in safe code (no undefined behavior)
2. No data races (checked at compile time)
3. No null pointer dereferences (except in unsafe code)
4. No buffer overflows (bounds checked)
5. No use-after-free (lifetime system)

**Performance Characteristics:**
1. Zero-cost abstractions (no runtime overhead)
2. Predictable performance (no hidden allocations)
3. Efficient memory layout (optimized by default)
4. Optional runtime checks (can be disabled in release mode)

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  

**Next**: [06-compilation.md](06-compilation.md) - Compilation Process and Architecture
