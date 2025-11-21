# Apex Compiler Test Suite

This directory contains comprehensive tests for all implemented features of the Apex compiler.

## Running Tests

To run all tests:
```bash
cd tests
./run_tests.sh
```

The test runner will:
1. Compile each `.apex` file
2. Link the object file to create an executable
3. Run the executable and verify the exit code matches the expected value
4. Report pass/fail for each test

## Test Files

### Loop Tests
- **for_basic.apex** - Basic for loop summing 0..9 (Expected: 45)
- **for_nested.apex** - Nested for loops with multiplication (Expected: 18)
- **while_basic.apex** - Basic while loop summing 0..9 (Expected: 45)
- **while_nested.apex** - Nested while loops (Expected: 12)

### Mutable Variable Tests
- **mut_basic.apex** - Basic mutable variable operations (Expected: 30)
- **mut_multiple.apex** - Multiple mutable variables (Expected: 14)

### Range Expression Tests
- **range_basic.apex** - Basic range 0..3 (Expected: 3)
- **range_offset.apex** - Range with offset 10..13 (Expected: 33)

### Struct Tests
- **struct_basic.apex** - Basic struct with fields (Expected: 25)
- **struct_nested.apex** - Nested struct types (Expected: 80)

## Test Results

All tests currently pass:
```
✓ PASS for_basic (exit code: 45)
✓ PASS for_nested (exit code: 18)
✓ PASS mut_basic (exit code: 30)
✓ PASS mut_multiple (exit code: 14)
✓ PASS range_basic (exit code: 3)
✓ PASS range_offset (exit code: 33)
✓ PASS struct_basic (exit code: 25)
✓ PASS struct_nested (exit code: 80)
✓ PASS while_basic (exit code: 45)
✓ PASS while_nested (exit code: 12)
```

## Features Verified

- ✅ While loops with conditions
- ✅ For loops with range expressions
- ✅ Mutable variables with `mut` keyword
- ✅ Range expressions (`..` operator)
- ✅ Struct type definitions
- ✅ Struct literals
- ✅ Field access
- ✅ Nested structs
- ✅ Nested loops
- ✅ Arithmetic operations
- ✅ Comparison operations

## Known Limitations

- Assignment operator (`=`) for reassigning mutable variables causes compiler crash (under investigation)
- Nested struct literals not yet supported (workaround: use intermediate variables)
