# Contributing to Apex

Thank you for your interest in contributing to Apex! This document provides guidelines for contributing to the project.

## Getting Started

1. Fork the repository from https://github.com/ShubhamPhapale/apex
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/apex.git`
3. Add upstream remote: `git remote add upstream https://github.com/ShubhamPhapale/apex.git`
4. Create a feature branch from `develop`: `git checkout develop && git checkout -b feature/your-feature-name`
5. Make your changes following our [Branching Strategy](BRANCHING_STRATEGY.md)
6. Test your changes: `cd tests && ./run_tests.sh`
7. Commit using conventional commits: `git commit -m "feat: add your feature"`
8. Push: `git push origin feature/your-feature-name`
9. Create a Pull Request to `develop` branch (not `main`)

**Important:** Read [BRANCHING_STRATEGY.md](BRANCHING_STRATEGY.md) for detailed workflow and commit conventions.

## Development Setup

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 13+, MSVC 19.29+)
- CMake 3.20+
- LLVM 14+
- Git

### Building
```bash
./build.sh
```

### Testing
```bash
# Comprehensive test suite (recommended - runs all 43 tests)
cd tests && ./run_tests.sh

# Quick smoke test (faster - tests basic functionality only)
./test.sh
```

## Code Style

### C++ Style
- Follow LLVM coding conventions
- Use 4 spaces for indentation (no tabs)
- Line limit: 100 characters (soft limit)
- Use `snake_case` for variables and functions
- Use `PascalCase` for classes and types
- Add comments for complex logic

Example:
```cpp
class SemanticAnalyzer {
public:
    bool analyze(ast::Module* module);
    
private:
    void analyze_function(ast::Item* func);
    std::vector<Symbol> symbols_;
};
```

### Apex Code Style
- Use 4 spaces for indentation
- Opening braces on same line
- One statement per line
- Descriptive variable names

Example:
```apex
fn calculate_sum(values: &[i32]) -> i32 {
    let mut sum: i32 = 0;
    for value in values {
        sum += value;
    }
    return sum;
}
```

## Project Areas

### Compiler
- **Lexer**: `src/apexc/lexer/` - Tokenization
- **Parser**: `src/apexc/parser/` - AST generation
- **Semantic Analysis**: `src/apexc/sema/` - Type checking, borrow checking
- **Code Generation**: `src/apexc/codegen/` - LLVM IR emission

### Standard Library
- **Core**: `stdlib/core/` - Core types and traits
- **Collections**: `stdlib/collections/` - Vec, HashMap, etc.
- **I/O**: `stdlib/io/` - File and console I/O
- **Networking**: `stdlib/net/` - TCP/UDP sockets

### Tooling
- **LSP**: `src/tools/apex-lsp/` - Language server
- **Formatter**: `src/tools/apex-fmt/` - Code formatter
- **Linter**: `src/tools/apex-lint/` - Static analyzer

## Testing

### Unit Tests
Place tests in `tests/unit/`:
```cpp
TEST_CASE("Lexer tokenizes integers") {
    apex::Lexer lexer("42", "<test>");
    auto token = lexer.next_token();
    REQUIRE(token.type == apex::TokenType::INTEGER_LITERAL);
    REQUIRE(token.lexeme == "42");
}
```

### Integration Tests
Place test files in `tests/integration/`:
```apex
// tests/integration/test_arithmetic.apx
fn test_addition() {
    assert_eq(2 + 2, 4);
}
```

### Running Tests
```bash
# Comprehensive test suite (43 tests - use this before submitting PR)
cd tests && ./run_tests.sh

# Quick smoke test (basic functionality check)
./test.sh

# Unit tests (when implemented)
./build/tests/unit_tests
```

## Documentation

### Code Comments
- Document all public APIs
- Explain non-obvious logic
- Include examples for complex functions

### Language Specification
- Update relevant .md files in `docs/spec/`
- Follow existing format and style
- Include code examples

## Pull Request Guidelines

### Before Submitting
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] New tests added for new features
- [ ] Documentation updated
- [ ] Code follows style guidelines
- [ ] Commit messages are descriptive

### PR Description Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
How was this tested?

## Checklist
- [ ] Code compiles
- [ ] Tests pass
- [ ] Documentation updated
```

## Reporting Issues

### Bug Reports
Include:
- Apex version: `apexc --version`
- Operating system
- LLVM version: `llvm-config --version`
- Minimal reproduction case
- Expected vs actual behavior

### Feature Requests
Include:
- Use case description
- Proposed syntax/API
- Impact on existing code
- Alternative approaches considered

## Code of Conduct

### Our Standards
- Be respectful and inclusive
- Welcome newcomers
- Accept constructive criticism
- Focus on what's best for the community

### Unacceptable Behavior
- Harassment or discrimination
- Trolling or insulting comments
- Publishing others' private information
- Other conduct inappropriate in a professional setting

## Questions?

- Open a GitHub Discussion for questions
- GitHub Issues: https://github.com/ShubhamPhapale/apex/issues
- Contact: Shubham Phapale

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Maintainer**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex
