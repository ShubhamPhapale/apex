# Apex Compiler - High Priority TODO List

**Version:** 1.0.0-alpha.2 → 1.0.0-beta  
**Target:** Q2 2025  
**Last Updated:** November 22, 2025

---

## 🎯 Phase 1: Complete Core Fundamentals (High Priority)

These are essential features needed to make Apex usable for basic systems programming.

### 1. Arrays (CRITICAL - Week 1-2)
**Priority:** CRITICAL  
**Status:** Parser only, no codegen  
**Effort:** 2-3 weeks

**Tasks:**
- [ ] Array type codegen in LLVM (fixed-size arrays)
- [ ] Array literal codegen `[1, 2, 3]`
- [ ] Array repeat syntax codegen `[0; 100]`
- [ ] Array indexing codegen `arr[i]`
- [ ] Array bounds checking (safe mode)
- [ ] Multi-dimensional arrays `[[i32; 3]; 3]`
- [ ] Add array tests (indexing, bounds, iteration)

**Blocker for:** Loops with collections, string operations, most real programs

**Example to support:**
```apex
fn main() -> i32 {
    let arr: [i32; 5] = [1, 2, 3, 4, 5];
    let sum: i32 = 0;
    for i in 0..5 {
        sum = sum + arr[i];
    }
    return sum;  // 15
}
```

---

### 2. Strings (CRITICAL - Week 3-4)
**Priority:** CRITICAL  
**Status:** Not implemented  
**Effort:** 3-4 weeks

**Tasks:**
- [ ] String type representation (pointer + length)
- [ ] String literals codegen `"hello"`
- [ ] String concatenation
- [ ] String comparison
- [ ] Basic string methods (len, slice, chars)
- [ ] C string interop (null-terminated)
- [ ] UTF-8 validation
- [ ] Add string tests

**Blocker for:** I/O, error messages, any text processing

**Example to support:**
```apex
fn greet(name: string) -> string {
    return "Hello, " + name + "!";
}
```

---

### 3. Logical Operators && and || (HIGH - Week 5)
**Priority:** HIGH  
**Status:** Not implemented  
**Effort:** 1 week

**Tasks:**
- [ ] Add && and || tokens to lexer (already done)
- [ ] Parse logical operators
- [ ] Implement short-circuit evaluation in codegen
- [ ] Add tests for logical operators

**Blocker for:** Complex conditionals, guard clauses

**Example to support:**
```apex
fn is_valid(x: i32, y: i32) -> bool {
    return x > 0 && y > 0 && x < 100 && y < 100;
}
```

---

### 4. Type Checker (HIGH - Week 6-8)
**Priority:** HIGH  
**Status:** 30% complete (basic structure only)  
**Effort:** 3-4 weeks

**Tasks:**
- [ ] Type inference for let bindings
- [ ] Type checking for binary operations
- [ ] Type checking for function calls (arg types match params)
- [ ] Type checking for return statements
- [ ] Type compatibility rules (implicit conversions)
- [ ] Generic type substitution
- [ ] Proper error messages with type mismatches
- [ ] Add type checking tests

**Blocker for:** Type safety, catching bugs at compile time

**Example to catch:**
```apex
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn main() -> i32 {
    let x: string = "hello";
    return add(x, 5);  // ERROR: expected i32, found string
}
```

---

### 5. Basic Standard Library - I/O (HIGH - Week 9-10)
**Priority:** HIGH  
**Status:** 0% (not started)  
**Effort:** 2-3 weeks

**Tasks:**
- [ ] std.io.print(string) - print without newline
- [ ] std.io.println(string) - print with newline
- [ ] std.io.print_i32(i32) - print integer
- [ ] std.io.read_line() -> string - read from stdin
- [ ] std.io.eprint(string) - print to stderr
- [ ] File I/O basics (open, read, write, close)
- [ ] Add I/O tests

**Blocker for:** Interactive programs, debugging, real-world use

**Example to support:**
```apex
import std.io;

fn main() -> i32 {
    io.print("Enter your name: ");
    let name: string = io.read_line();
    io.println("Hello, " + name + "!");
    return 0;
}
```

---

### 6. Enums (MEDIUM - Week 11-12)
**Priority:** MEDIUM  
**Status:** Parser only, no codegen  
**Effort:** 2-3 weeks

**Tasks:**
- [ ] Enum type codegen
- [ ] Enum variant codegen
- [ ] Enum pattern matching in match expressions
- [ ] Enum with data (variant payloads)
- [ ] Add enum tests

**Blocker for:** Result<T, E>, Optional<T>, error handling

**Example to support:**
```apex
enum Result<T, E> {
    Ok(T),
    Err(E)
}

fn divide(a: i32, b: i32) -> Result<i32, string> {
    if b == 0 {
        return Result.Err("Division by zero");
    } else {
        return Result.Ok(a / b);
    }
}
```

---

### 7. Tuples (MEDIUM - Week 13)
**Priority:** MEDIUM  
**Status:** Parser only, no codegen  
**Effort:** 1-2 weeks

**Tasks:**
- [ ] Tuple type codegen `(i32, string, bool)`
- [ ] Tuple literal codegen `(1, "hello", true)`
- [ ] Tuple indexing `tuple.0`, `tuple.1`
- [ ] Tuple destructuring in let statements
- [ ] Add tuple tests

**Blocker for:** Multiple return values, compound data

**Example to support:**
```apex
fn swap(a: i32, b: i32) -> (i32, i32) {
    return (b, a);
}

fn main() -> i32 {
    let pair: (i32, i32) = (10, 20);
    let (x, y) = swap(pair.0, pair.1);
    return x + y;  // 30
}
```

---

### 8. References and Borrowing (HIGH - Week 14-16)
**Priority:** HIGH  
**Status:** Parser only, no borrow checking  
**Effort:** 3-4 weeks

**Tasks:**
- [ ] Reference type codegen `&T`, `&mut T`
- [ ] Reference creation codegen `&x`, `&mut x`
- [ ] Reference dereferencing codegen `*ptr`
- [ ] Basic borrow checker (no multiple mutable borrows)
- [ ] Lifetime analysis (basic)
- [ ] Add borrow checking tests

**Blocker for:** Zero-copy operations, memory safety

**Example to support:**
```apex
fn increment(x: &mut i32) {
    *x = *x + 1;
}

fn main() -> i32 {
    let mut val: i32 = 10;
    increment(&mut val);
    return val;  // 11
}
```

---

### 9. Slices (MEDIUM - Week 17)
**Priority:** MEDIUM  
**Status:** Not implemented  
**Effort:** 1-2 weeks

**Tasks:**
- [ ] Slice type codegen `[]T`
- [ ] Array to slice coercion `&arr[0..5]`
- [ ] Slice indexing
- [ ] Slice iteration in for loops
- [ ] Add slice tests

**Blocker for:** Dynamic arrays, string slices, flexible APIs

**Example to support:**
```apex
fn sum(numbers: []i32) -> i32 {
    let total: i32 = 0;
    for i in 0..numbers.len() {
        total = total + numbers[i];
    }
    return total;
}
```

---

### 10. Generic Functions (HIGH - Week 18-20)
**Priority:** HIGH  
**Status:** Parser only, no monomorphization  
**Effort:** 3-4 weeks

**Tasks:**
- [ ] Generic function instantiation
- [ ] Monomorphization (generate concrete versions)
- [ ] Generic type constraints (trait bounds)
- [ ] Generic struct instantiation
- [ ] Add generic tests

**Blocker for:** Reusable data structures, standard library

**Example to support:**
```apex
fn max<T>(a: T, b: T) -> T {
    if a > b {
        return a;
    } else {
        return b;
    }
}

fn main() -> i32 {
    let x: i32 = max(10, 20);  // monomorphize to max_i32
    let y: f32 = max(1.5, 2.5);  // monomorphize to max_f32
    return x;
}
```

---

## 🚀 Phase 2: Standard Library Essentials (Week 21-28)

### 11. std.mem - Memory Management
- [ ] Box<T> - heap allocation
- [ ] Rc<T> - reference counting
- [ ] Arc<T> - atomic reference counting
- [ ] Memory allocation functions (alloc, dealloc)

### 12. std.collections - Data Structures
- [ ] Vec<T> - dynamic array
- [ ] HashMap<K, V> - hash map
- [ ] HashSet<T> - hash set
- [ ] LinkedList<T> - linked list

### 13. std.string - String Operations
- [ ] String type (owned, growable)
- [ ] String methods (push, pop, trim, split, etc.)
- [ ] Format strings
- [ ] String conversion traits

### 14. std.fs - File System
- [ ] File operations (open, read, write, close)
- [ ] Directory operations (create, list, remove)
- [ ] Path manipulation
- [ ] File metadata

---

## 📝 Phase 3: Advanced Features (Week 29-40)

### 15. Traits (Week 29-32)
- [ ] Trait definition codegen
- [ ] Trait implementation codegen
- [ ] Trait bounds in generics
- [ ] Dynamic dispatch (trait objects)

### 16. Error Handling (Week 33-34)
- [ ] Result<T, E> enum
- [ ] Optional<T> enum
- [ ] ? operator for error propagation
- [ ] Panic mechanism

### 17. Modules (Week 35-36)
- [ ] Module system implementation
- [ ] Import/export resolution
- [ ] Module privacy (pub)
- [ ] Multi-file projects

### 18. Unsafe Blocks (Week 37-38)
- [ ] Unsafe block parsing and codegen
- [ ] Raw pointers (*const T, *mut T)
- [ ] Unsafe operations (deref raw pointers)
- [ ] FFI support

### 19. Pointers (Week 39-40)
- [ ] Raw pointer operations
- [ ] Pointer arithmetic
- [ ] Null pointer handling
- [ ] Pointer safety checks in safe code

---

## 🔧 Phase 4: Compiler Improvements (Week 41-48)

### 20. Optimization Passes
- [ ] Dead code elimination
- [ ] Constant folding
- [ ] Inline expansion
- [ ] Loop optimization

### 21. Better Error Messages
- [ ] Source code snippets in errors
- [ ] Suggestions for fixes
- [ ] Color-coded output
- [ ] Error explanations

### 22. Debug Information
- [ ] DWARF debug info generation
- [ ] Source line mapping
- [ ] Variable names in debugger
- [ ] Stack traces

---

## 🎯 Success Criteria for Beta Release

### Must Have (v1.0.0-beta)
- ✅ All 43 current tests passing
- [ ] Arrays working with indexing
- [ ] Strings with basic operations
- [ ] Logical operators (&&, ||)
- [ ] Type checker catching common errors
- [ ] Basic I/O (print, println, read_line)
- [ ] Enums with pattern matching
- [ ] References and basic borrowing
- [ ] Generic functions with monomorphization
- [ ] 100+ tests covering all features
- [ ] Can compile "real" programs (>500 LOC)

### Should Have (v1.0.0-beta)
- [ ] Tuples working
- [ ] Slices working
- [ ] Box<T> for heap allocation
- [ ] Vec<T> for dynamic arrays
- [ ] Basic traits (Display, Debug)
- [ ] Error handling with Result<T, E>
- [ ] Module system for multi-file projects

### Nice to Have (v1.0.0-beta)
- [ ] HashMap, HashSet
- [ ] File I/O
- [ ] Better error messages
- [ ] Optimization passes
- [ ] Debug information

---

## 📊 Priority Matrix

| Feature | Priority | Effort | Impact | Dependencies |
|---------|----------|--------|--------|--------------|
| Arrays | CRITICAL | Medium | High | None |
| Strings | CRITICAL | Medium | High | Arrays |
| Logical Operators | HIGH | Low | Medium | None |
| Type Checker | HIGH | High | High | None |
| I/O Library | HIGH | Medium | High | Strings |
| Enums | MEDIUM | Medium | High | Pattern matching |
| Tuples | MEDIUM | Low | Medium | None |
| References | HIGH | High | High | Borrow checker |
| Slices | MEDIUM | Low | Medium | Arrays, References |
| Generics | HIGH | High | High | Type checker |

---

## 🗓️ Recommended Development Order

**Weeks 1-5: Core Language Features**
1. Arrays (2 weeks)
2. Strings (2 weeks)
3. Logical operators (1 week)

**Weeks 6-10: Type System & I/O**
4. Type checker (3 weeks)
5. Basic I/O library (2 weeks)

**Weeks 11-17: Advanced Types**
6. Enums (2 weeks)
7. Tuples (1 week)
8. References & borrowing (3 weeks)
9. Slices (1 week)

**Weeks 18-20: Generics**
10. Generic functions & monomorphization (3 weeks)

**Weeks 21-28: Standard Library**
11. Memory management (Box, Rc, Arc)
12. Collections (Vec, HashMap, HashSet)
13. More string operations
14. File system operations

---

## 📋 Current Blockers

### Immediate Blockers
1. **Arrays** - Can't iterate over collections properly
2. **Strings** - Can't do any text processing
3. **Type checker** - Accepting invalid programs

### Medium-term Blockers
1. **I/O** - Can't write interactive programs
2. **Enums** - Can't do proper error handling
3. **Generics** - Can't write reusable code

### Long-term Blockers
1. **Borrow checker** - Not memory safe yet
2. **Standard library** - Missing essential utilities
3. **Module system** - Can't organize large projects

---

## 🎯 Next Immediate Steps (This Week)

### Day 1-2: Arrays - Foundation
- [ ] Implement fixed-size array type in codegen
- [ ] Array literal codegen
- [ ] Basic array indexing

### Day 3-4: Arrays - Operations
- [ ] Array repeat syntax
- [ ] Array bounds checking
- [ ] Array in loops

### Day 5-7: Arrays - Testing & Polish
- [ ] Write 10+ array tests
- [ ] Fix bugs found in testing
- [ ] Update documentation
- [ ] Commit and push changes

**After arrays are complete, move to strings immediately.**

---

## 📊 Metrics to Track

- [ ] Test count (target: 100+ by beta)
- [ ] Lines of Apex code compiled successfully (target: 1000+ by beta)
- [ ] Compiler warnings fixed (target: 0)
- [ ] TODOs in code (target: reduce by 50%)
- [ ] Documentation coverage (target: 100% for implemented features)

---

**Let's build the future of systems programming! 🚀**
