# 3. Syntax Specification with Examples

## Hello World

```apex
import std.io;

fn main() -> i32 {
    io.println("Hello, World!");
    return 0;
}
```

**With command-line arguments:**
```apex
import std.io;

fn main(args: []string) -> i32 {
    if args.len > 1 {
        io.println("Hello, {}!", args[1]);
    } else {
        io.println("Hello, World!");
    }
    return 0;
}
```

## Variables & Types

### Variable Declaration

```apex
// Immutable by default
let x = 42;
let y: i32 = 100;

// Mutable variables
let mut count = 0;
count += 1;

// Constants (compile-time evaluated)
const MAX_SIZE: usize = 1024;
const PI: f64 = 3.14159265359;
```

### Type Annotations

```apex
let explicit_int: i32 = 42;
let inferred_int = 42;        // i32

let explicit_float: f64 = 3.14;
let inferred_float = 3.14;    // f64

let character: char = 'A';
let boolean: bool = true;
```

### Tuples

```apex
let pair: (i32, i32) = (10, 20);
let triple = (1, 2.5, "hello");

// Destructuring
let (x, y) = pair;
let (a, b, c) = triple;

// Accessing by index
let first = pair.0;
let second = pair.1;
```

### Type Casting

```apex
let x: i32 = 42;
let y: i64 = x as i64;        // Explicit cast
let z: f64 = x as f64;

// Truncating cast (potentially lossy)
let big: i64 = 1000000000000;
let small: i32 = big as i32;   // Warning: may overflow

// Safe conversions
let result: Result<i32, Error> = i32.try_from(big);
```

## Conditionals & Loops

### If-Else

```apex
let x = 10;

if x > 0 {
    io.println("Positive");
} else if x < 0 {
    io.println("Negative");
} else {
    io.println("Zero");
}

// If as expression
let sign = if x > 0 { "positive" } else if x < 0 { "negative" } else { "zero" };
```

### Match (Pattern Matching)

```apex
let number = 3;

match number {
    1 => io.println("One"),
    2 => io.println("Two"),
    3 => io.println("Three"),
    _ => io.println("Other"),
}

// Match with multiple patterns
match number {
    1 | 2 | 3 => io.println("Small"),
    4 | 5 | 6 => io.println("Medium"),
    _ => io.println("Large"),
}

// Match with guards
match number {
    n if n < 0 => io.println("Negative"),
    n if n > 100 => io.println("Large positive"),
    _ => io.println("Normal range"),
}

// Match returning values
let description = match number {
    1 => "one",
    2 => "two",
    3 => "three",
    _ => "many",
};
```

### While Loop

```apex
let mut count = 0;

while count < 10 {
    io.println("Count: {}", count);
    count += 1;
}

// While with break/continue
while true {
    let input = io.read_line();
    if input == "quit" {
        break;
    }
    if input.is_empty() {
        continue;
    }
    process(input);
}
```

### For Loop

```apex
// Range-based for
for i in 0..10 {
    io.println("i = {}", i);
}

// Inclusive range
for i in 0..=10 {
    io.println("i = {}", i);
}

// Iterating over arrays
let numbers = [1, 2, 3, 4, 5];
for num in numbers {
    io.println("Number: {}", num);
}

// Enumerate (index + value)
for (i, value) in numbers.enumerate() {
    io.println("Index {}: {}", i, value);
}

// Infinite loop
for {
    if should_stop() {
        break;
    }
    work();
}
```

### Loop Labels

```apex
'outer: for i in 0..10 {
    for j in 0..10 {
        if i * j > 50 {
            break 'outer;  // Break outer loop
        }
        io.println("{} * {} = {}", i, j, i * j);
    }
}
```

## Functions

### Basic Functions

```apex
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

// Implicit return (last expression)
fn multiply(a: i32, b: i32) -> i32 {
    a * b  // No semicolon, this is returned
}

// No return value
fn print_sum(a: i32, b: i32) {
    io.println("Sum: {}", a + b);
}
```

### Multiple Return Values

```apex
fn divide_with_remainder(dividend: i32, divisor: i32) -> (i32, i32) {
    return (dividend / divisor, dividend % divisor);
}

let (quotient, remainder) = divide_with_remainder(17, 5);
```

### Default Parameters

```apex
fn greet(name: string, greeting: string = "Hello") {
    io.println("{}, {}!", greeting, name);
}

greet("Alice");              // "Hello, Alice!"
greet("Bob", "Hi");          // "Hi, Bob!"
```

### Variadic Functions

```apex
fn sum_all(values: ...i32) -> i32 {
    let total = 0;
    for val in values {
        total += val;
    }
    return total;
}

let result = sum_all(1, 2, 3, 4, 5);  // 15
```

### Generic Functions

```apex
fn swap<T>(a: &mut T, b: &mut T) {
    let temp = *a;
    *a = *b;
    *b = temp;
}

let mut x = 5;
let mut y = 10;
swap(&mut x, &mut y);
```

### Higher-Order Functions

```apex
fn apply<T>(f: fn(T) -> T, value: T) -> T {
    return f(value);
}

fn double(x: i32) -> i32 { return x * 2; }

let result = apply(double, 21);  // 42
```

### Lambda/Anonymous Functions

```apex
let square = |x: i32| -> i32 { x * x };
let result = square(5);  // 25

// With type inference
let add = |a, b| a + b;

// Capturing environment
let multiplier = 10;
let scale = |x| x * multiplier;
let scaled = scale(5);  // 50
```

## Structs & Enums

### Struct Definition

```apex
struct Point {
    x: i32,
    y: i32,
}

// Creating instances
let p1 = Point { x: 10, y: 20 };
let p2 = Point { x: 0, y: 0 };

// Accessing fields
let x_coord = p1.x;

// Mutable struct
let mut p3 = Point { x: 5, y: 5 };
p3.x = 15;
```

### Struct Methods

```apex
impl Point {
    // Constructor (associated function)
    fn new(x: i32, y: i32) -> Point {
        return Point { x: x, y: y };
    }
    
    // Method (takes self)
    fn distance_from_origin(&self) -> f64 {
        let x_sq = (self.x * self.x) as f64;
        let y_sq = (self.y * self.y) as f64;
        return math.sqrt(x_sq + y_sq);
    }
    
    // Mutable method
    fn translate(&mut self, dx: i32, dy: i32) {
        self.x += dx;
        self.y += dy;
    }
    
    // Consuming method (takes ownership)
    fn into_tuple(self) -> (i32, i32) {
        return (self.x, self.y);
    }
}

// Usage
let p = Point.new(3, 4);
let distance = p.distance_from_origin();

let mut p2 = Point.new(0, 0);
p2.translate(5, 10);
```

### Tuple Structs

```apex
struct Color(u8, u8, u8);

let red = Color(255, 0, 0);
let green_channel = red.1;
```

### Enums

```apex
enum Direction {
    North,
    South,
    East,
    West,
}

let dir = Direction.North;

match dir {
    Direction.North => io.println("Going north"),
    Direction.South => io.println("Going south"),
    Direction.East => io.println("Going east"),
    Direction.West => io.println("Going west"),
}
```

### Enums with Data

```apex
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(string),
    ChangeColor(u8, u8, u8),
}

let msg1 = Message.Quit;
let msg2 = Message.Move { x: 10, y: 20 };
let msg3 = Message.Write("Hello");
let msg4 = Message.ChangeColor(255, 0, 0);

match msg2 {
    Message.Quit => io.println("Quit"),
    Message.Move { x, y } => io.println("Move to ({}, {})", x, y),
    Message.Write(text) => io.println("Write: {}", text),
    Message.ChangeColor(r, g, b) => io.println("Color: ({}, {}, {})", r, g, b),
}
```

### Optional Type

```apex
enum Optional<T> {
    Some(T),
    None,
}

fn find_user(id: i32) -> Optional<string> {
    if id == 1 {
        return Some("Alice");
    }
    return None;
}

let user = find_user(1);
match user {
    Some(name) => io.println("Found: {}", name),
    None => io.println("Not found"),
}

// Using if-let
if let Some(name) = find_user(1) {
    io.println("User: {}", name);
}
```

### Result Type

```apex
enum Result<T, E> {
    Ok(T),
    Err(E),
}

fn divide(a: i32, b: i32) -> Result<i32, string> {
    if b == 0 {
        return Err("Division by zero");
    }
    return Ok(a / b);
}

let result = divide(10, 2);
match result {
    Ok(value) => io.println("Result: {}", value),
    Err(msg) => io.println("Error: {}", msg),
}

// Using unwrap (panics on error)
let value = divide(10, 2).unwrap();

// Using unwrap_or (provides default)
let value = divide(10, 0).unwrap_or(-1);
```

## Memory Allocation

### Stack Allocation (Default)

```apex
fn stack_example() {
    let x = 42;                          // Stack
    let arr: [i32; 100] = [0; 100];     // Stack
    let point = Point { x: 10, y: 20 };  // Stack
}  // All cleaned up automatically
```

### Heap Allocation

```apex
import std.mem;

fn heap_example() {
    // Allocate single value
    let ptr: *mut Point = mem.alloc<Point>();
    unsafe {
        *ptr = Point { x: 10, y: 20 };
        io.println("Point: ({}, {})", (*ptr).x, (*ptr).y);
        mem.dealloc(ptr);
    }
    
    // Allocate array
    let arr: *mut i32 = mem.alloc_array<i32>(100);
    unsafe {
        for i in 0..100 {
            *(arr + i) = i as i32;
        }
        mem.dealloc(arr);
    }
}
```

### Smart Pointers

```apex
import std.mem.{Box, Rc, Arc};

fn smart_pointer_example() {
    // Box: Owned heap pointer
    let boxed = Box.new(Point { x: 1, y: 2 });
    io.println("Point: ({}, {})", boxed.x, boxed.y);
    // Automatically freed when boxed goes out of scope
    
    // Rc: Reference counted (single-threaded)
    let rc1 = Rc.new(42);
    let rc2 = rc1.clone();  // Increment reference count
    io.println("Value: {}", *rc1);
    // Freed when last reference goes out of scope
    
    // Arc: Atomic reference counted (thread-safe)
    let arc = Arc.new(100);
    thread.spawn(|| {
        io.println("Value: {}", *arc.clone());
    });
}
```

### Custom Allocators

```apex
struct ArenaAllocator {
    buffer: *mut u8,
    size: usize,
    used: usize,
}

impl ArenaAllocator {
    fn new(size: usize) -> ArenaAllocator {
        return ArenaAllocator {
            buffer: unsafe { mem.alloc_array<u8>(size) },
            size: size,
            used: 0,
        };
    }
    
    fn alloc<T>(&mut self) -> *mut T {
        let type_size = size_of<T>();
        let alignment = align_of<T>();
        
        // Align pointer
        let aligned = (self.used + alignment - 1) & !(alignment - 1);
        
        if aligned + type_size > self.size {
            return null;
        }
        
        let ptr = unsafe { self.buffer + aligned };
        self.used = aligned + type_size;
        return ptr as *mut T;
    }
    
    fn reset(&mut self) {
        self.used = 0;
    }
    
    fn destroy(&mut self) {
        unsafe { mem.dealloc(self.buffer); }
    }
}
```

## Modules and Imports

### Module Definition

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

pub fn cross(a: Vector3, b: Vector3) -> Vector3 {
    return Vector3 {
        x: a.y * b.z - a.z * b.y,
        y: a.z * b.x - a.x * b.z,
        z: a.x * b.y - a.y * b.x,
    };
}

// Private function
fn magnitude_squared(v: Vector3) -> f32 {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

pub fn magnitude(v: Vector3) -> f32 {
    return math.sqrt(magnitude_squared(v));
}
```

### Importing Modules

```apex
// Import entire module
import math.vector;

let v1 = vector.Vector3 { x: 1.0, y: 0.0, z: 0.0 };
let v2 = vector.Vector3 { x: 0.0, y: 1.0, z: 0.0 };
let cross_product = vector.cross(v1, v2);

// Import specific items
import math.vector.{Vector3, dot, cross};

let v1 = Vector3 { x: 1.0, y: 0.0, z: 0.0 };
let result = dot(v1, v1);

// Import with alias
import math.vector as vec;

let v = vec.Vector3 { x: 1.0, y: 2.0, z: 3.0 };

// Import all public items
import math.vector.*;

let v = Vector3 { x: 1.0, y: 2.0, z: 3.0 };
```

### Nested Modules

```apex
// File: graphics/shapes/circle.apex
module graphics.shapes.circle;

pub struct Circle {
    pub center: Point,
    pub radius: f32,
}

// File: main.apex
import graphics.shapes.circle.Circle;

let c = Circle {
    center: Point { x: 0, y: 0 },
    radius: 10.0,
};
```

### Visibility Modifiers

```apex
module mymodule;

pub fn public_function() { }           // Visible everywhere
fn private_function() { }              // Module-private
pub(crate) fn crate_function() { }     // Visible in same crate
pub(super) fn parent_function() { }    // Visible to parent module
```

## Concurrency

### Thread Creation

```apex
import std.thread;
import std.io;

fn worker(id: i32) {
    io.println("Worker {} started", id);
    thread.sleep_ms(1000);
    io.println("Worker {} finished", id);
}

fn main() {
    let mut threads = Vec.new<Thread>();
    
    for i in 0..4 {
        let t = thread.spawn(|| worker(i));
        threads.push(t);
    }
    
    for t in threads {
        t.join().unwrap();
    }
    
    io.println("All workers finished");
}
```

### Message Passing (Channels)

```apex
import std.sync.Channel;
import std.thread;

fn main() {
    let (tx, rx) = Channel.new<i32>();
    
    thread.spawn(|| {
        for i in 0..10 {
            tx.send(i).unwrap();
            thread.sleep_ms(100);
        }
    });
    
    for received in rx {
        io.println("Received: {}", received);
    }
}
```

### Shared State (Mutex)

```apex
import std.sync.{Mutex, Arc};
import std.thread;

fn main() {
    let counter = Arc.new(Mutex.new(0));
    let mut threads = Vec.new<Thread>();
    
    for _ in 0..10 {
        let counter_clone = counter.clone();
        let t = thread.spawn(|| {
            let mut guard = counter_clone.lock().unwrap();
            *guard += 1;
        });
        threads.push(t);
    }
    
    for t in threads {
        t.join().unwrap();
    }
    
    io.println("Final count: {}", *counter.lock().unwrap());
}
```

### Atomic Operations

```apex
import std.sync.atomic.{AtomicI32, Ordering};

let counter = AtomicI32.new(0);

fn increment(counter: &AtomicI32) {
    counter.fetch_add(1, Ordering.SeqCst);
}

// Use in multiple threads
let counter_ref = &counter;
thread.spawn(|| increment(counter_ref));
thread.spawn(|| increment(counter_ref));
```

## I/O Operations

### Console I/O

```apex
import std.io;

fn main() {
    // Print without newline
    io.print("Enter your name: ");
    
    // Print with newline
    io.println("Hello, World!");
    
    // Formatted output
    let name = "Alice";
    let age = 30;
    io.println("Name: {}, Age: {}", name, age);
    
    // Read line from stdin
    let input = io.read_line().unwrap();
    io.println("You entered: {}", input);
    
    // Read until EOF
    let contents = io.read_to_string().unwrap();
}
```

### File Operations

```apex
import std.fs.File;
import std.io;

fn read_file_example(path: string) -> Result<string, Error> {
    let file = File.open(path)?;
    defer file.close();
    
    let contents = file.read_to_string()?;
    return Ok(contents);
}

fn write_file_example(path: string, data: string) -> Result<void, Error> {
    let file = File.create(path)?;
    defer file.close();
    
    file.write(data.as_bytes())?;
    return Ok(void);
}

fn append_file_example(path: string, data: string) -> Result<void, Error> {
    let file = File.open_append(path)?;
    defer file.close();
    
    file.write(data.as_bytes())?;
    return Ok(void);
}

fn main() {
    // Read file
    match read_file_example("input.txt") {
        Ok(contents) => io.println("File contents: {}", contents),
        Err(e) => io.println("Error: {}", e),
    }
    
    // Write file
    write_file_example("output.txt", "Hello, World!").unwrap();
    
    // Append to file
    append_file_example("log.txt", "New entry\n").unwrap();
}
```

### Buffered I/O

```apex
import std.io.{BufReader, BufWriter};
import std.fs.File;

fn read_lines(path: string) -> Result<Vec<string>, Error> {
    let file = File.open(path)?;
    defer file.close();
    
    let reader = BufReader.new(file);
    let lines = Vec.new<string>();
    
    for line in reader.lines() {
        lines.push(line?);
    }
    
    return Ok(lines);
}

fn write_lines(path: string, lines: []string) -> Result<void, Error> {
    let file = File.create(path)?;
    defer file.close();
    
    let mut writer = BufWriter.new(file);
    
    for line in lines {
        writer.write(line.as_bytes())?;
        writer.write(b"\n")?;
    }
    
    writer.flush()?;
    return Ok(void);
}
```

### Directory Operations

```apex
import std.fs;

fn main() {
    // Create directory
    fs.create_dir("new_directory").unwrap();
    
    // Create directory and parents
    fs.create_dir_all("path/to/directory").unwrap();
    
    // Read directory
    let entries = fs.read_dir(".").unwrap();
    for entry in entries {
        let entry = entry.unwrap();
        io.println("{}", entry.path());
    }
    
    // Remove directory
    fs.remove_dir("old_directory").unwrap();
    
    // Remove directory and contents
    fs.remove_dir_all("old_directory").unwrap();
    
    // Check if path exists
    if fs.exists("file.txt") {
        io.println("File exists");
    }
    
    // Get metadata
    let metadata = fs.metadata("file.txt").unwrap();
    io.println("Size: {} bytes", metadata.len());
}
```

## Safe/Unsafe Blocks

### Safe Mode (Default)

```apex
fn safe_function() {
    let arr = [1, 2, 3, 4, 5];
    
    // Bounds checked
    let value = arr[2];  // OK
    // let invalid = arr[10];  // Runtime panic
    
    // Overflow checked
    let x: i32 = 2000000000;
    // let y = x + x;  // Runtime panic
    
    // References always valid
    let r = &arr[0];
    io.println("Value: {}", *r);
}
```

### Unsafe Blocks

```apex
fn unsafe_example() {
    let arr = [1, 2, 3, 4, 5];
    
    unsafe {
        // Raw pointer manipulation
        let ptr = arr.as_ptr();
        let value = *(ptr + 2);  // No bounds check
        
        // Unchecked array access
        let fast_value = arr.unchecked_get(2);
        
        // Type punning
        let int_ptr = ptr as *const i32;
        let byte_ptr = int_ptr as *const u8;
        
        // Call unsafe function
        raw_memory_operation(ptr);
    }
}

unsafe fn raw_memory_operation(ptr: *const i32) {
    // Can perform unsafe operations
}
```

### FFI with C

```apex
// Declare external C functions
extern "C" {
    fn malloc(size: usize) -> *mut void;
    fn free(ptr: *mut void);
    fn printf(format: *const char, ...) -> i32;
}

fn use_c_functions() {
    unsafe {
        let ptr = malloc(100);
        if ptr != null {
            // Use allocated memory
            free(ptr);
        }
        
        printf(c"Hello from C!\n");
    }
}
```

### Inline Assembly

```apex
unsafe fn read_cpu_id() -> u32 {
    let result: u32;
    asm!(
        "mov {}, eax",
        out(reg) result,
    );
    return result;
}

unsafe fn write_port(port: u16, value: u8) {
    asm!(
        "out dx, al",
        in("dx") port,
        in("al") value,
    );
}
```

## Brief Sample Project

### TCP Echo Server

```apex
import std.net.{TcpListener, TcpStream};
import std.io;
import std.thread;

fn handle_client(mut stream: TcpStream) {
    let peer = stream.peer_addr().unwrap();
    io.println("New connection from {}", peer);
    
    let mut buffer: [u8; 1024] = [0; 1024];
    
    loop {
        match stream.read(&mut buffer) {
            Ok(0) => break,  // Connection closed
            Ok(n) => {
                // Echo back what was received
                stream.write(&buffer[..n]).unwrap();
            },
            Err(e) => {
                io.println("Error reading from {}: {}", peer, e);
                break;
            },
        }
    }
    
    io.println("Connection closed: {}", peer);
}

fn main() -> Result<void, Error> {
    let listener = TcpListener.bind("127.0.0.1:8080")?;
    io.println("Server listening on {}", listener.local_addr()?);
    
    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                thread.spawn(|| handle_client(stream));
            },
            Err(e) => {
                io.println("Connection failed: {}", e);
            },
        }
    }
    
    return Ok(void);
}
```

### Project Structure

```
echo_server/
├── project.apex          # Build configuration
├── src/
│   ├── main.apex        # Main entry point
│   ├── server.apex      # Server logic
│   └── protocol.apex    # Protocol handling
└── tests/
    └── integration.apex  # Integration tests
```

**project.apex:**
```apex
project {
    name: "echo_server",
    version: "1.0.0",
    authors: ["Alice <alice@example.com>"],
    
    dependencies: {
        "std": "1.0",
    },
    
    targets: {
        executable: {
            name: "echo_server",
            entry: "src/main.apex",
        },
    },
    
    build: {
        optimization: "release",
        debug_symbols: true,
    },
}
```

---

**Next**: [04-grammar.md](04-grammar.md) - Formal EBNF Grammar
