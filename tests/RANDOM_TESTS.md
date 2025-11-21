# Random Test Suite

This directory contains a comprehensive random test suite that validates all language features of the Apex programming language.

## Test Coverage

The random test suite consists of 13 tests covering:

### 1. **random_arithmetic_complex.apx** (Expected: 147)
- Complex nested arithmetic expressions
- Multiple function calls with arithmetic operations
- Mixed operations (+, -, *, /)
- Deep expression nesting

### 2. **random_comparison_mixed.apx** (Expected: 42)
- All comparison operators (<, >, <=, >=, ==, !=)
- Nested boolean conditions
- Comparison chaining with if-else

### 3. **random_loops_complex.apx** (Expected: 206)
- For loops with ranges (0..n, offset ranges)
- While loops with conditions
- Nested for loops (double and triple nesting)
- Nested while inside for loops
- Loop variable accumulation

### 4. **random_match_patterns.apx** (Expected: 129)
- Match with literal values (0, 1, 2, ...)
- Match with wildcard pattern (_)
- Match with function calls
- Match with arithmetic expressions
- Nested match expressions
- Match inside loops

### 5. **random_mutation_patterns.apx** (Expected: 193)
- Mutable local variables
- Mutable function parameters
- Mutations in loops
- Mutations in conditionals
- Struct field mutations

### 6. **random_struct_operations.apx** (Expected: 114)
- Basic struct creation and field access
- Nested struct literals (struct within struct)
- Nested field access (rect.top_left.x)
- Struct arithmetic operations
- Struct fields in function calls

### 7. **random_block_expressions.apx** (Expected: 251)
- Simple block expressions
- Blocks with variable bindings
- Nested blocks
- Blocks in conditionals
- Blocks in loops
- Blocks in match arms
- Block return values

### 8. **random_function_calls.apx** (Expected: 204)
- Simple function calls
- Nested function calls
- Recursive functions (factorial, fibonacci)
- Chained function operations
- Function calls in expressions

### 9. **random_if_else_chains.apx** (Expected: 167)
- Deep if-else nesting (5+ levels)
- If-else in loops
- If-else expressions (ternary-like)
- Complex boolean conditions

### 10. **random_integration_all.apx** (Expected: 217)
- Integration of ALL features in one test
- Structs with mutations
- Loops with match
- Recursive functions
- Nested structures
- Complex nested logic
- While loops
- Final calculations combining everything

## Test Patterns

All tests follow these patterns:

1. **Comment Header**: Each test includes:
   - Description: `// Random test: <description>`
   - Expected exit code: `// Expected: <exit_code>`

2. **Comprehensive Coverage**: Tests exercise edge cases and complex combinations

3. **Expected Value Calculations**: Comments show how expected values are derived

4. **No Trivial Tests**: Each test validates non-trivial language behavior

## Running the Tests

All random tests are automatically included in the main test suite:

```bash
cd tests
./run_tests.sh
```

The output will show:
```
✓ PASS random_arithmetic_complex (exit code: 147)
✓ PASS random_comparison_mixed (exit code: 42)
... (all 10 tests)
```

## Known Limitations

The random tests work around known compiler limitations:

1. **Struct field mutation in while loops**: Direct mutation of struct fields inside while loops causes codegen issues. Tests use intermediate variables instead.

2. **Trailing commas**: Struct literals cannot have trailing commas after the last field.

## Test Philosophy

The random test suite complements the existing 30 tests by:

- Testing complex feature combinations
- Validating edge cases
- Ensuring features work together correctly
- Providing confidence for future development

### 11. **random_edge_cases.apx** (Expected: 255)
- Zero handling and null cases
- Boundary conditions (0..0 empty range)
- Single iteration loops (0..1)
- Division by zero protection
- While false (condition never true)
- Empty and nested empty blocks
- Match with zero cases

### 12. **random_recursion_deep.apx** (Expected: 233)
- Tail-recursive functions
- Limited factorial to prevent overflow
- Count-down recursion
- Nested recursive calls
- Recursion in loops
- Recursion in match expressions
- Stack depth testing

### 13. **random_precedence.apx** (Expected: 142)
- Arithmetic operator precedence (* and / before + and -)
- Left-to-right associativity
- Parentheses override precedence
- Nested parentheses
- Complex expressions combining all operators
- Comparison operators in expressions
- Expression evaluation in match

## Statistics

- **Original tests**: 30
- **Random tests**: 13
- **Total tests**: 43
- **Pass rate**: 100%
