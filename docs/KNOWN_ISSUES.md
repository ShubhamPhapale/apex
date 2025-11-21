# Known Issues and Limitations

**Last Updated:** November 22, 2025  
**Version:** 1.0.0-alpha.2

## ✅ Recently Fixed Issues

### 1. If/Match Expression Results ✅ FIXED
**Fixed in:** v1.0.0-alpha.2  
**Description:** Match expressions in loops were causing LLVM verification failures due to allocas being created inside loop blocks.

**Solution:** Match result allocas now created at function entry block using temporary IRBuilder, ensuring proper SSA form.

---

### 2. Mutable Function Parameters ✅ FIXED
**Fixed in:** v1.0.0-alpha.2  
**Description:** Parser couldn't handle `mut` keyword in function parameters, causing infinite loops.

**Solution:** Changed parse_function_params() to use parse_pattern() instead of consume(IDENTIFIER), enabling proper pattern matching for parameters.

---

### 3. Mutable Parameter Mutations ✅ FIXED
**Fixed in:** v1.0.0-alpha.2  
**Description:** Mutable function parameters were treated as immutable SSA values.

**Solution:** Mutable parameters now get allocas at function entry, arguments stored, and allocas used in named_allocas_ map for mutation support.

---

## Current Limitations

### 1. Logical Operators Not Implemented
**Severity:** Medium  
**Status:** Not Implemented  
**Operators:** `&&`, `||`  
**Workaround:** Use nested if statements or bitwise operators for boolean logic

---

### 2. Struct Field Mutation in While Loops
**Severity:** Low  
**Status:** Known Limitation  
**Description:** Direct mutation of struct fields inside while loops causes codegen issues (missing basic block terminator).

**Workaround:** Use intermediate variables
```apex
// Instead of:
while data.value > 0 {
    data.value = data.value - 1;
}

// Use:
let mut temp: i32 = data.value;
while temp > 0 {
    temp = temp - 1;
}
data.value = temp;
```

---

### 3. Trailing Commas in Struct Literals
**Severity:** Low  
**Status:** Parser Limitation  
**Description:** Struct literals cannot have trailing commas after the last field.

```apex
// Works:
let p = Point { x: 1, y: 2 };

// Fails:
let p = Point { x: 1, y: 2, };
```

---

## Test Results

**Test Suite Status:** 43/43 passing (100%) ✅

### Passing Tests (43)
- ✅ All 30 original tests passing
- ✅ All 13 random tests passing
- ✅ arithmetic_*, assignment_*, block_*, comparison_*
- ✅ comprehensive_integration, for_*, function_*
- ✅ if_*, match_*, mutation_*, range_*, struct_*, while_*
- ✅ All stress tests (comparison_stress, if_stress, match_stress, mutation_stress)
- ✅ Random tests: arithmetic_complex, comparison_mixed, loops_complex
- ✅ Random tests: match_patterns, mutation_patterns, struct_operations
- ✅ Random tests: block_expressions, function_calls, if_else_chains
- ✅ Random tests: integration_all, edge_cases, recursion_deep, precedence

### Failing Tests
**None** - All 43 tests passing!

---

**Author:** Shubham Phapale  
**Repository:** https://github.com/ShubhamPhapale/apex
