# 7. Standard Library Specification

## Overview

The Apex standard library (`std`) provides essential functionality for systems programming while maintaining zero-cost abstractions and portability across platforms.

**Design Principles:**
1. **Minimal Core:** Small, essential API surface
2. **Zero-Cost:** No runtime overhead for abstractions
3. **Safe Defaults:** Unsafe operations explicitly marked
4. **Portable:** Works across all supported platforms
5. **Modular:** Import only what you need

## Module Structure

```
std/
├── alloc      - Memory allocation
├── collections - Data structures
├── env        - Environment variables
├── error      - Error handling
├── fs         - File system operations
├── io         - Input/output primitives
├── iter       - Iterators
├── math       - Mathematical functions
├── mem        - Memory manipulation
├── net        - Networking
├── ops        - Operator traits
├── os         - OS-specific functionality
├── panic      - Panic handling
├── path       - Path manipulation
├── process    - Process management
├── ptr        - Pointer utilities
├── string     - String types and manipulation
├── sync       - Synchronization primitives
├── thread     - Threading
└── time       - Time and duration
```

## Core Utilities (std.core)

### Primitive Types

```apex
module std.core;

// Integer types
pub type i8, i16, i32, i64, i128, isize;
pub type u8, u16, u32, u64, u128, usize;

// Floating-point types
pub type f32, f64;

// Other primitives
pub type bool, char, byte;

// Unit type
pub type void = ();
```

### Option Type

```apex
pub enum Optional<T> {
    Some(T),
    None,
}

impl<T> Optional<T> {
    pub fn is_some(&self) -> bool;
    pub fn is_none(&self) -> bool;
    pub fn unwrap(self) -> T;
    pub fn unwrap_or(self, default: T) -> T;
    pub fn map<U>(self, f: fn(T) -> U) -> Optional<U>;
    pub fn and_then<U>(self, f: fn(T) -> Optional<U>) -> Optional<U>;
}
```

### Result Type

```apex
pub enum Result<T, E> {
    Ok(T),
    Err(E),
}

impl<T, E> Result<T, E> {
    pub fn is_ok(&self) -> bool;
    pub fn is_err(&self) -> bool;
    pub fn unwrap(self) -> T;
    pub fn unwrap_or(self, default: T) -> T;
    pub fn map<U>(self, f: fn(T) -> U) -> Result<U, E>;
    pub fn map_err<F>(self, f: fn(E) -> F) -> Result<T, F>;
}
```

## Memory Management (std.mem)

### Allocation

```apex
module std.mem;

// Low-level allocation
pub unsafe fn alloc<T>() -> *mut T;
pub unsafe fn alloc_array<T>(count: usize) -> *mut T;
pub unsafe fn alloc_zeroed<T>() -> *mut T;
pub unsafe fn realloc<T>(ptr: *mut T, new_count: usize) -> *mut T;
pub unsafe fn dealloc<T>(ptr: *mut T);

// Memory operations
pub unsafe fn copy<T>(src: *const T, dst: *mut T, count: usize);
pub unsafe fn copy_nonoverlapping<T>(src: *const T, dst: *mut T, count: usize);
pub unsafe fn write<T>(dst: *mut T, val: T);
pub unsafe fn read<T>(src: *const T) -> T;
pub unsafe fn swap<T>(a: *mut T, b: *mut T);

// Type information
pub fn size_of<T>() -> usize;
pub fn align_of<T>() -> usize;
pub fn size_of_val<T>(val: &T) -> usize;
pub fn align_of_val<T>(val: &T) -> usize;
```

### Smart Pointers

```apex
// Box: Owned heap allocation
pub struct Box<T> {
    ptr: *mut T,
}

impl<T> Box<T> {
    pub fn new(value: T) -> Box<T>;
    pub fn into_raw(self) -> *mut T;
    pub unsafe fn from_raw(ptr: *mut T) -> Box<T>;
}

// Reference counted pointer (single-threaded)
pub struct Rc<T> {
    ptr: *mut RcBox<T>,
}

impl<T> Rc<T> {
    pub fn new(value: T) -> Rc<T>;
    pub fn clone(&self) -> Rc<T>;
    pub fn strong_count(&self) -> usize;
    pub fn weak_count(&self) -> usize;
}

// Atomic reference counted pointer (thread-safe)
pub struct Arc<T> {
    ptr: *mut ArcInner<T>,
}

impl<T> Arc<T> {
    pub fn new(value: T) -> Arc<T>;
    pub fn clone(&self) -> Arc<T>;
    pub fn strong_count(&self) -> usize;
    pub fn weak_count(&self) -> usize;
}
```

## Math Library (std.math)

### Constants

```apex
module std.math;

pub const PI: f64 = 3.14159265358979323846;
pub const E: f64 = 2.71828182845904523536;
pub const TAU: f64 = 6.28318530717958647692;
pub const SQRT_2: f64 = 1.41421356237309504880;
```

### Basic Functions

```apex
// Trigonometric
pub fn sin(x: f64) -> f64;
pub fn cos(x: f64) -> f64;
pub fn tan(x: f64) -> f64;
pub fn asin(x: f64) -> f64;
pub fn acos(x: f64) -> f64;
pub fn atan(x: f64) -> f64;
pub fn atan2(y: f64, x: f64) -> f64;

// Exponential and logarithmic
pub fn exp(x: f64) -> f64;
pub fn ln(x: f64) -> f64;
pub fn log10(x: f64) -> f64;
pub fn log2(x: f64) -> f64;
pub fn pow(base: f64, exp: f64) -> f64;
pub fn sqrt(x: f64) -> f64;
pub fn cbrt(x: f64) -> f64;

// Rounding
pub fn floor(x: f64) -> f64;
pub fn ceil(x: f64) -> f64;
pub fn round(x: f64) -> f64;
pub fn trunc(x: f64) -> f64;

// Other
pub fn abs<T>(x: T) -> T where T: Numeric;
pub fn min<T>(a: T, b: T) -> T where T: Ord;
pub fn max<T>(a: T, b: T) -> T where T: Ord;
pub fn clamp<T>(value: T, min: T, max: T) -> T where T: Ord;
```

## File I/O (std.fs)

### File Operations

```apex
module std.fs;

pub struct File {
    fd: i32,
}

impl File {
    pub fn open(path: string) -> Result<File, IoError>;
    pub fn create(path: string) -> Result<File, IoError>;
    pub fn open_append(path: string) -> Result<File, IoError>;
    pub fn read(&mut self, buf: &mut [u8]) -> Result<usize, IoError>;
    pub fn write(&mut self, buf: &[u8]) -> Result<usize, IoError>;
    pub fn read_to_string(&mut self) -> Result<string, IoError>;
    pub fn write_all(&mut self, buf: &[u8]) -> Result<void, IoError>;
    pub fn seek(&mut self, pos: SeekFrom) -> Result<u64, IoError>;
    pub fn sync_all(&mut self) -> Result<void, IoError>;
    pub fn close(self) -> Result<void, IoError>;
}

pub enum SeekFrom {
    Start(u64),
    End(i64),
    Current(i64),
}
```

### Directory Operations

```apex
// Create directory
pub fn create_dir(path: string) -> Result<void, IoError>;
pub fn create_dir_all(path: string) -> Result<void, IoError>;

// Remove directory
pub fn remove_dir(path: string) -> Result<void, IoError>;
pub fn remove_dir_all(path: string) -> Result<void, IoError>;

// Read directory
pub fn read_dir(path: string) -> Result<ReadDir, IoError>;

pub struct ReadDir {
    // Platform-specific
}

impl ReadDir {
    pub fn next(&mut self) -> Optional<Result<DirEntry, IoError>>;
}

pub struct DirEntry {
    pub fn path(&self) -> string;
    pub fn file_name(&self) -> string;
    pub fn file_type(&self) -> Result<FileType, IoError>;
}
```

### File Metadata

```apex
pub fn metadata(path: string) -> Result<Metadata, IoError>;

pub struct Metadata {
    pub fn len(&self) -> u64;
    pub fn is_dir(&self) -> bool;
    pub fn is_file(&self) -> bool;
    pub fn is_symlink(&self) -> bool;
    pub fn permissions(&self) -> Permissions;
    pub fn modified(&self) -> Result<SystemTime, IoError>;
    pub fn accessed(&self) -> Result<SystemTime, IoError>;
    pub fn created(&self) -> Result<SystemTime, IoError>;
}

pub struct Permissions {
    pub fn readonly(&self) -> bool;
    pub fn set_readonly(&mut self, readonly: bool);
}
```

## I/O Primitives (std.io)

### Console I/O

```apex
module std.io;

// Standard output
pub fn print(s: string);
pub fn println(s: string);
pub fn eprint(s: string);  // stderr
pub fn eprintln(s: string);

// Formatted output
pub fn printf(format: string, ...args);
pub fn eprintf(format: string, ...args);

// Standard input
pub fn read_line() -> Result<string, IoError>;
pub fn read_to_string() -> Result<string, IoError>;
```

### Buffered I/O

```apex
pub struct BufReader<R> {
    inner: R,
    buf: Vec<u8>,
    pos: usize,
    cap: usize,
}

impl<R: Read> BufReader<R> {
    pub fn new(inner: R) -> BufReader<R>;
    pub fn with_capacity(capacity: usize, inner: R) -> BufReader<R>;
    pub fn read_line(&mut self) -> Result<string, IoError>;
    pub fn lines(&mut self) -> Lines<BufReader<R>>;
}

pub struct BufWriter<W> {
    inner: W,
    buf: Vec<u8>,
}

impl<W: Write> BufWriter<W> {
    pub fn new(inner: W) -> BufWriter<W>;
    pub fn with_capacity(capacity: usize, inner: W) -> BufWriter<W>;
    pub fn flush(&mut self) -> Result<void, IoError>;
}
```

### Traits

```apex
pub trait Read {
    fn read(&mut self, buf: &mut [u8]) -> Result<usize, IoError>;
}

pub trait Write {
    fn write(&mut self, buf: &[u8]) -> Result<usize, IoError>;
    fn flush(&mut self) -> Result<void, IoError>;
}

pub trait Seek {
    fn seek(&mut self, pos: SeekFrom) -> Result<u64, IoError>;
}
```

## Time Library (std.time)

### Duration

```apex
module std.time;

pub struct Duration {
    secs: u64,
    nanos: u32,
}

impl Duration {
    pub fn from_secs(secs: u64) -> Duration;
    pub fn from_millis(millis: u64) -> Duration;
    pub fn from_micros(micros: u64) -> Duration;
    pub fn from_nanos(nanos: u64) -> Duration;
    
    pub fn as_secs(&self) -> u64;
    pub fn as_millis(&self) -> u128;
    pub fn as_micros(&self) -> u128;
    pub fn as_nanos(&self) -> u128;
}
```

### SystemTime

```apex
pub struct SystemTime {
    // Platform-specific
}

impl SystemTime {
    pub fn now() -> SystemTime;
    pub fn elapsed(&self) -> Result<Duration, SystemTimeError>;
    pub fn duration_since(&self, earlier: SystemTime) -> Result<Duration, SystemTimeError>;
}
```

### Instant (Monotonic Clock)

```apex
pub struct Instant {
    // Platform-specific
}

impl Instant {
    pub fn now() -> Instant;
    pub fn elapsed(&self) -> Duration;
    pub fn duration_since(&self, earlier: Instant) -> Duration;
}
```

## Threading (std.thread)

### Thread Creation

```apex
module std.thread;

pub struct Thread {
    // Platform-specific
}

pub fn spawn<F>(f: F) -> Thread 
where F: FnOnce() + Send;

impl Thread {
    pub fn join(self) -> Result<void, JoinError>;
    pub fn id(&self) -> ThreadId;
}

// Current thread operations
pub fn current() -> Thread;
pub fn sleep(dur: Duration);
pub fn sleep_ms(ms: u64);
pub fn yield_now();
pub fn park();
pub fn unpark(thread: &Thread);
```

### Thread Local Storage

```apex
pub struct ThreadLocal<T> {
    // Platform-specific
}

impl<T> ThreadLocal<T> {
    pub fn new() -> ThreadLocal<T>;
    pub fn with<F, R>(&self, f: F) -> R where F: FnOnce(&T) -> R;
}
```

## Synchronization (std.sync)

### Mutex

```apex
module std.sync;

pub struct Mutex<T> {
    data: T,
    lock: AtomicBool,
}

impl<T> Mutex<T> {
    pub fn new(value: T) -> Mutex<T>;
    pub fn lock(&self) -> MutexGuard<T>;
    pub fn try_lock(&self) -> Result<MutexGuard<T>, TryLockError>;
}

pub struct MutexGuard<'a, T> {
    // Automatically unlocks on drop
}
```

### RwLock

```apex
pub struct RwLock<T> {
    data: T,
    // Platform-specific
}

impl<T> RwLock<T> {
    pub fn new(value: T) -> RwLock<T>;
    pub fn read(&self) -> RwLockReadGuard<T>;
    pub fn write(&self) -> RwLockWriteGuard<T>;
    pub fn try_read(&self) -> Result<RwLockReadGuard<T>, TryLockError>;
    pub fn try_write(&self) -> Result<RwLockWriteGuard<T>, TryLockError>;
}
```

### Channels

```apex
pub struct Channel<T> {
    // Platform-specific
}

pub fn channel<T>() -> (Sender<T>, Receiver<T>);

pub struct Sender<T> {
    pub fn send(&self, value: T) -> Result<void, SendError<T>>;
    pub fn clone(&self) -> Sender<T>;
}

pub struct Receiver<T> {
    pub fn recv(&self) -> Result<T, RecvError>;
    pub fn try_recv(&self) -> Result<T, TryRecvError>;
    pub fn iter(&self) -> Iter<T>;
}
```

### Atomic Types

```apex
pub struct AtomicBool {
    pub fn new(value: bool) -> AtomicBool;
    pub fn load(&self, order: Ordering) -> bool;
    pub fn store(&self, value: bool, order: Ordering);
    pub fn swap(&self, value: bool, order: Ordering) -> bool;
    pub fn compare_exchange(&self, current: bool, new: bool, 
                           success: Ordering, failure: Ordering) 
                           -> Result<bool, bool>;
}

pub struct AtomicI32 {
    pub fn new(value: i32) -> AtomicI32;
    pub fn load(&self, order: Ordering) -> i32;
    pub fn store(&self, value: i32, order: Ordering);
    pub fn swap(&self, value: i32, order: Ordering) -> i32;
    pub fn fetch_add(&self, val: i32, order: Ordering) -> i32;
    pub fn fetch_sub(&self, val: i32, order: Ordering) -> i32;
    pub fn compare_exchange(&self, current: i32, new: i32,
                           success: Ordering, failure: Ordering)
                           -> Result<i32, i32>;
}

// Similar for: AtomicU32, AtomicI64, AtomicU64, AtomicUsize

pub enum Ordering {
    Relaxed,
    Acquire,
    Release,
    AcqRel,
    SeqCst,
}
```

## Networking (std.net)

### TCP

```apex
module std.net;

pub struct TcpListener {
    fd: i32,
}

impl TcpListener {
    pub fn bind(addr: string) -> Result<TcpListener, IoError>;
    pub fn accept(&self) -> Result<(TcpStream, SocketAddr), IoError>;
    pub fn incoming(&self) -> Incoming;
    pub fn local_addr(&self) -> Result<SocketAddr, IoError>;
}

pub struct TcpStream {
    fd: i32,
}

impl TcpStream {
    pub fn connect(addr: string) -> Result<TcpStream, IoError>;
    pub fn read(&mut self, buf: &mut [u8]) -> Result<usize, IoError>;
    pub fn write(&mut self, buf: &[u8]) -> Result<usize, IoError>;
    pub fn peer_addr(&self) -> Result<SocketAddr, IoError>;
    pub fn local_addr(&self) -> Result<SocketAddr, IoError>;
    pub fn shutdown(&self, how: Shutdown) -> Result<void, IoError>;
}
```

### UDP

```apex
pub struct UdpSocket {
    fd: i32,
}

impl UdpSocket {
    pub fn bind(addr: string) -> Result<UdpSocket, IoError>;
    pub fn send_to(&self, buf: &[u8], addr: string) -> Result<usize, IoError>;
    pub fn recv_from(&self, buf: &mut [u8]) -> Result<(usize, SocketAddr), IoError>;
    pub fn local_addr(&self) -> Result<SocketAddr, IoError>;
}
```

### Socket Addresses

```apex
pub enum SocketAddr {
    V4(SocketAddrV4),
    V6(SocketAddrV6),
}

pub struct SocketAddrV4 {
    pub fn new(ip: Ipv4Addr, port: u16) -> SocketAddrV4;
    pub fn ip(&self) -> Ipv4Addr;
    pub fn port(&self) -> u16;
}

pub struct SocketAddrV6 {
    pub fn new(ip: Ipv6Addr, port: u16) -> SocketAddrV6;
    pub fn ip(&self) -> Ipv6Addr;
    pub fn port(&self) -> u16;
}
```

## Collections (std.collections)

### Vector

```apex
module std.collections;

pub struct Vec<T> {
    ptr: *mut T,
    len: usize,
    capacity: usize,
}

impl<T> Vec<T> {
    pub fn new() -> Vec<T>;
    pub fn with_capacity(capacity: usize) -> Vec<T>;
    pub fn push(&mut self, value: T);
    pub fn pop(&mut self) -> Optional<T>;
    pub fn len(&self) -> usize;
    pub fn capacity(&self) -> usize;
    pub fn is_empty(&self) -> bool;
    pub fn clear(&mut self);
    pub fn get(&self, index: usize) -> Optional<&T>;
    pub fn get_mut(&mut self, index: usize) -> Optional<&mut T>;
    pub fn insert(&mut self, index: usize, value: T);
    pub fn remove(&mut self, index: usize) -> T;
}
```

### HashMap

```apex
pub struct HashMap<K, V> {
    // Internal implementation
}

impl<K, V> HashMap<K, V> where K: Hash + Eq {
    pub fn new() -> HashMap<K, V>;
    pub fn with_capacity(capacity: usize) -> HashMap<K, V>;
    pub fn insert(&mut self, key: K, value: V) -> Optional<V>;
    pub fn get(&self, key: &K) -> Optional<&V>;
    pub fn get_mut(&mut self, key: &K) -> Optional<&mut V>;
    pub fn remove(&mut self, key: &K) -> Optional<V>;
    pub fn contains_key(&self, key: &K) -> bool;
    pub fn len(&self) -> usize;
    pub fn is_empty(&self) -> bool;
    pub fn clear(&mut self);
}
```

### HashSet

```apex
pub struct HashSet<T> {
    map: HashMap<T, ()>,
}

impl<T> HashSet<T> where T: Hash + Eq {
    pub fn new() -> HashSet<T>;
    pub fn insert(&mut self, value: T) -> bool;
    pub fn remove(&mut self, value: &T) -> bool;
    pub fn contains(&self, value: &T) -> bool;
    pub fn len(&self) -> usize;
    pub fn is_empty(&self) -> bool;
    pub fn clear(&mut self);
}
```

### LinkedList

```apex
pub struct LinkedList<T> {
    // Internal implementation
}

impl<T> LinkedList<T> {
    pub fn new() -> LinkedList<T>;
    pub fn push_front(&mut self, value: T);
    pub fn push_back(&mut self, value: T);
    pub fn pop_front(&mut self) -> Optional<T>;
    pub fn pop_back(&mut self) -> Optional<T>;
    pub fn front(&self) -> Optional<&T>;
    pub fn back(&self) -> Optional<&T>;
    pub fn len(&self) -> usize;
    pub fn is_empty(&self) -> bool;
}
```

### BinaryHeap

```apex
pub struct BinaryHeap<T> {
    data: Vec<T>,
}

impl<T> BinaryHeap<T> where T: Ord {
    pub fn new() -> BinaryHeap<T>;
    pub fn push(&mut self, value: T);
    pub fn pop(&mut self) -> Optional<T>;
    pub fn peek(&self) -> Optional<&T>;
    pub fn len(&self) -> usize;
    pub fn is_empty(&self) -> bool;
}
```

## String Types (std.string)

### String

```apex
module std.string;

pub struct String {
    vec: Vec<u8>,  // UTF-8 encoded
}

impl String {
    pub fn new() -> String;
    pub fn from(s: string) -> String;
    pub fn with_capacity(capacity: usize) -> String;
    pub fn push(&mut self, ch: char);
    pub fn push_str(&mut self, s: string);
    pub fn pop(&mut self) -> Optional<char>;
    pub fn len(&self) -> usize;
    pub fn is_empty(&self) -> bool;
    pub fn clear(&mut self);
    pub fn as_bytes(&self) -> &[u8];
    pub fn chars(&self) -> Chars;
}
```

### String Operations

```apex
// String slicing (UTF-8 aware)
pub fn substr(s: string, start: usize, len: usize) -> string;

// String searching
pub fn contains(haystack: string, needle: string) -> bool;
pub fn starts_with(s: string, prefix: string) -> bool;
pub fn ends_with(s: string, suffix: string) -> bool;
pub fn find(haystack: string, needle: string) -> Optional<usize>;

// String manipulation
pub fn to_uppercase(s: string) -> String;
pub fn to_lowercase(s: string) -> String;
pub fn trim(s: string) -> string;
pub fn split(s: string, delimiter: string) -> Vec<string>;
pub fn join(parts: []string, separator: string) -> String;
pub fn replace(s: string, from: string, to: string) -> String;
```

## Error Handling (std.error)

### Error Trait

```apex
module std.error;

pub trait Error {
    fn message(&self) -> string;
    fn source(&self) -> Optional<&dyn Error>;
}

pub struct ErrorChain {
    errors: Vec<Box<dyn Error>>,
}

impl ErrorChain {
    pub fn new(error: Box<dyn Error>) -> ErrorChain;
    pub fn chain(self, error: Box<dyn Error>) -> ErrorChain;
    pub fn iter(&self) -> ErrorChainIter;
}
```

## Process Management (std.process)

### Command Execution

```apex
module std.process;

pub struct Command {
    program: string,
    args: Vec<string>,
    env: HashMap<string, string>,
    cwd: Optional<string>,
}

impl Command {
    pub fn new(program: string) -> Command;
    pub fn arg(&mut self, arg: string) -> &mut Command;
    pub fn args(&mut self, args: []string) -> &mut Command;
    pub fn env(&mut self, key: string, val: string) -> &mut Command;
    pub fn current_dir(&mut self, dir: string) -> &mut Command;
    pub fn spawn(&mut self) -> Result<Child, IoError>;
    pub fn output(&mut self) -> Result<Output, IoError>;
    pub fn status(&mut self) -> Result<ExitStatus, IoError>;
}

pub struct Child {
    pub fn wait(&mut self) -> Result<ExitStatus, IoError>;
    pub fn kill(&mut self) -> Result<void, IoError>;
}

pub struct Output {
    pub status: ExitStatus,
    pub stdout: Vec<u8>,
    pub stderr: Vec<u8>,
}
```

### Process Exit

```apex
pub fn exit(code: i32) -> !;
pub fn abort() -> !;
```

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  

**Next**: [08-examples.md](08-examples.md) - Complete Example Programs
