# Known Issues and Limitations

**Last Updated:** November 21, 2025  
**Version:** 1.0.0-alpha

## Critical Issues

### 1. If/Match Expression Results Cause Segfault ⚠️ CRITICAL
**Severity:** Critical  
**Status:** Bug - Needs Fix  
**Description:** Assigning if-expression or match-expression results to variables causes compiler segfault during codegen.

**Examples that FAIL:**
```apex
// Fails - if-expression assigned to variable
let x: i32 = if a < b { 1 } else { 2 };

// Fails - match-expression assigned to variable
let y: i32 = match n {
    0 => 10,
    _ => 20,
};

// Fails - if-expression returned directly  
return if a < b { 1 } else { 2 };
```

**Workaround:** Use if-statements with mutation instead
```apex
let mut x: i32 = 0;
if a < b {
    x = 1;
} else {
    x = 2;
}
```

**Root Cause:** PHI node creation issue in codegen

---

### 2. Logical Operators Not Implemented
**Severity:** High  
**Status:** Not Implemented  
**Operators:** `&&`, `||`  
**Workaround:** Use nested if statements

---

### 3. Struct Definitions Must Come Before main()
**Severity:** Medium  
**Description:** Struct definitions after main() cause verification failures

---

## Test Results

**Test Suite Status:** 23/30 passing (76.7%)

### Passing Tests (23)
All basic feature tests + stress tests that don't use if/match expressions

### Failing Tests (7)
- ❌ comparison_stress (uses if-expressions - segfault)
- ❌ if_stress (uses if-expressions - segfault)
- ❌ match_stress (uses if-expressions - segfault)
- ❌ comprehensive_integration (uses if/match expressions - segfault)
- ⏳ mutation_stress (hangs - needs investigation)
- ⏳ struct_stress (hangs - needs investigation)
- ⏳ while_basic (hangs - needs investigation)

---

**Author:** Shubham Phapale  
**Repository:** https://github.com/ShubhamPhapale/apex
