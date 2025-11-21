# Bug Report: Compiler Segfault with Multiple Let Statements + If Conditions

## Status
**Current Test Results**: 26/30 passing (86.7%)  
**Blocked Tests**: 4 (if_stress, comparison_stress, match_stress, mutation_stress)

## Summary
Compiler segfaults during compilation when:
1. Function contains 2+ let statements (immutable or mutable)
2. Variables from those let statements are used in an if-expression condition
3. The condition involves a binary comparison (e.g., `a < b`)

## Minimal Reproduction

### ✅ WORKS (1 let):
```apex
fn main() -> i32 {
    let a = 10;
    if a < 20 {
        50
    } else {
        30
    }
}
```

### ❌ CRASHES (2+ lets):
```apex
fn main() -> i32 {
    let a = 10;
    let b = 20;
    if a < b {
        50
    } else {
        30
    }
}
```

## Test Results

| Pattern | Result |
|---------|--------|
| 1 let + use in if condition | ✅ WORKS |
| 2 lets + use in if condition | ❌ SEGFAULT |
| 3 lets + use in if condition | ❌ SEGFAULT |
| 3 lets + `if true` (no var use) | ✅ WORKS |
| 2 lets + no if | ✅ WORKS |
| nested if-expressions | ✅ WORKS |

## Key Observations

1. **Not about nesting**: Initially thought to be nested if-expressions, but that's a red herring
2. **Not about statement count**: Works fine with 3+ lets if we don't use variables in if condition
3. **Not about mutability**: Crashes with both `let` and `let mut`
4. **Happens during compilation**: Crash occurs while generating LLVM IR, not at runtime

## Likely Causes

Without debugger access, potential issues include:

1. **LLVM IRBuilder state corruption**: Multiple SSA value lookups may corrupt builder state
2. **Invalid Value* usage**: Named_values_ map may contain stale LLVM Value* pointers
3. **std::unordered_map corruption**: Hash map with string keys might have iterator invalidation
4. **BasicBlock insertion point**: Builder insertion point may be invalid after multiple let statements

## Code Locations

- **Let statement codegen**: `LLVMCodeGen.cpp:772-807`
- **Identifier lookup**: `LLVMCodeGen.cpp:296-320`
- **Binary operation codegen**: `LLVMCodeGen.cpp:322-377`
- **If-expression codegen**: `LLVMCodeGen.cpp:413-472`

## Next Steps

**REQUIRES DEBUGGER**:
```bash
lldb build/src/apexc/apexc
(lldb) run tests/test_two_lets_use.apx -o /tmp/test.o
# Get backtrace at crash point
(lldb) bt
```

This will reveal:
- Exact function/line where segfault occurs
- LLVM API call that's failing
- State of IRBuilder, named_values_, and LLVM Value* objects

## Workaround

For now, tests can be rewritten to avoid the pattern:
- Use single let with tuple unpacking (if supported)
- Inline variable values directly in conditions
- Use intermediate function calls

## Impact

4 out of 30 tests blocked (13.3% failure rate). All blocked tests follow the "stress test" pattern with multiple variables and complex control flow.
