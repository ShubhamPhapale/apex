# Known Issues and Limitations

**Last Updated:** November 21, 2025  
**Version:** 1.0.0-alpha

## Critical Issues Found During Stress Testing

### 1. Unary Minus Not Implemented in Codegen
**Severity:** High  
**Status:** Not fixed  
**Description:** Unary minus operator (`-expr`, `-(expr)`) causes codegen failure.

**Example:**
```apex
fn test() -> i32 {
    let x: i32 = -5;  // Causes codegen failure
    return x;
}
```

**Workaround:** Use subtraction from zero: `0 - 5` instead of `-5`

**Files Affected:**
- `src/apexc/codegen/LLVMCodeGen.cpp` - Missing case for `ExprKind::Unary` with `UnOp::Neg`

---

### 2. Let Statements Without Explicit Return
**Severity:** Medium  
**Status:** Needs investigation  
**Description:** Multiple `let` statements followed by complex expressions sometimes fail codegen.

**Example:**
```apex
fn test() -> i32 {
    let a: i32 = 10;
    let b: i32 = 20;
    let c: i32 = 30;
    return a + b + c;  // Sometimes fails
}
```

**Workaround:** Keep functions simple with fewer intermediate variables

---

### 3. Modulo Operator Edge Cases
**Severity:** Low  
**Status:** Under investigation  
**Description:** Some complex expressions involving modulo (`%`) produce unexpected results.

**Example:**
```apex
let x: i32 = 100 + 20 - 10 * 2 / 4 % 3;  // May not match expected value
```

---

### 4. Logical Operators (&&, ||) Segfault
**Severity:** High  
**Status:** Not fixed  
**Description:** Using logical AND (`&&`) and OR (`||`) operators causes segmentation fault during compilation.

**Example:**
```apex
fn test() -> i32 {
    if (x > 5) && (x < 10) {  // Causes segfault
        return 1;
    }
    return 0;
}
```

**Workaround:** Use nested if statements
```apex
fn test() -> i32 {
    if x > 5 {
        if x < 10 {
            return 1;
        }
    }
    return 0;
}
```

---

### 5. Nested Block Scoping Issues
**Severity:** Medium  
**Status:** Partial - some cases work  
**Description:** Deeply nested blocks with multiple variables may produce incorrect results.

---

## Parser Limitations

### 1. Import Statement Infinite Loop
**Severity:** Critical (Fixed - Documented)  
**Status:** Workaround documented  
**Description:** Using `.` instead of `::` in import paths causes infinite loop.

**Correct Syntax:**
```apex
import std::io;  // Correct - uses ::
```

**Incorrect Syntax:**
```apex
import std.io;  // Wrong - causes infinite loop
```

---

## Features Not Yet Implemented

### High Priority
- [ ] Unary operators in codegen (-, !, ~)
- [ ] Logical operators codegen (&&, ||)
- [ ] Array types and literals
- [ ] Enum support in codegen
- [ ] Module system
- [ ] Standard library

### Medium Priority
- [ ] Generic functions and types
- [ ] Trait system
- [ ] Type aliases
- [ ] Better error recovery in parser

### Low Priority
- [ ] Closures
- [ ] Async/await
- [ ] Macros

---

## Test Suite Status

### Passing Tests (17/30)
- ✅ All basic feature tests (16/16)
- ✅ arithmetic_stress

### Failing Tests (13/30)
- ❌ comparison_stress (logical operators)
- ❌ block_stress (nested scoping)
- ❌ loop_stress (off-by-one)
- ❌ struct_stress (compilation failure)
- ❌ match_stress (incorrect results)
- ❌ mutation_stress (linking failure)
- ❌ function_stress (incorrect results)
- ❌ if_stress (incorrect results)
- ❌ edge_zero (linking failure)
- ❌ edge_negative (unary minus)
- ❌ edge_large (incorrect calculation)
- ❌ edge_single (linking failure)
- ❌ comprehensive_integration (compilation failure)

---

## Recommendations for Development

1. **Immediate:** Fix unary minus operator in codegen
2. **Immediate:** Implement logical operators (&&, ||) in codegen
3. **Short-term:** Improve error recovery in parser
4. **Medium-term:** Complete array implementation
5. **Long-term:** Implement module system and standard library

---

## Testing Guidelines

Until the above issues are fixed, write tests that:
- ✅ Use simple arithmetic expressions
- ✅ Avoid unary minus (use subtraction instead)
- ✅ Avoid logical operators (use nested ifs)
- ✅ Keep functions simple with minimal variables
- ✅ Test one feature at a time

---

**Author:** Shubham Phapale  
**Repository:** https://github.com/ShubhamPhapale/apex
