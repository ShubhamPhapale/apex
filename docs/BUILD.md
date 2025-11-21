# Apex Programming Language - Build Instructions

## Prerequisites

### macOS
```bash
brew install llvm cmake
export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake llvm-14 llvm-14-dev clang-14
```

### Windows (WSL)
Follow the Linux instructions above in WSL2.

## Building from Source

### Quick Build
```bash
chmod +x build.sh
./build.sh
```

### Manual Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)
```

### Build Options
- `APEX_BUILD_TESTS=ON/OFF` - Build test suite (default: ON)
- `APEX_BUILD_TOOLS=ON/OFF` - Build LSP and other tools (default: ON)
- `APEX_BUILD_EXAMPLES=ON/OFF` - Build example programs (default: ON)

Example:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DAPEX_BUILD_TESTS=OFF
```

## Testing the Compiler

### Compile a simple program
```bash
./build/src/apexc/apexc examples/hello.apex
```

### View LLVM IR
```bash
./build/src/apexc/apexc --emit-llvm examples/hello.apex
cat hello.ll
```

### View tokens
```bash
./build/src/apexc/apexc --emit-tokens examples/hello.apex
```

### View AST
```bash
./build/src/apexc/apexc --emit-ast examples/hello.apex
```

### Verbose output
```bash
./build/src/apexc/apexc -v examples/hello.apex
```

## Installation

```bash
cd build
sudo cmake --install .
```

This will install:
- `apexc` compiler to `/usr/local/bin/apexc`

## Project Structure

```
apex/
├── src/
│   └── apexc/              # Compiler implementation
│       ├── lexer/          # Tokenization
│       ├── parser/         # Parsing & AST
│       ├── ast/            # AST definitions
│       ├── sema/           # Semantic analysis
│       ├── codegen/        # LLVM code generation
│       └── main.cpp        # Compiler driver
├── examples/               # Example Apex programs
├── tests/                  # Test suite
├── docs/                   # Documentation
├── CMakeLists.txt         # Root build configuration
├── build.sh               # Build script
└── README.md              # Project overview
```

## Development

### Running Tests
```bash
cd build
ctest --output-on-failure
```

### Debugging
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
lldb ./src/apexc/apexc -- ../examples/hello.apex
```

### Code Style
The project uses:
- C++20 standard
- LLVM coding conventions
- 4-space indentation
- 80-character line limit (soft)

## Troubleshooting

### LLVM not found
If CMake can't find LLVM, set the LLVM_DIR variable:
```bash
export LLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm  # macOS
export LLVM_DIR=/usr/lib/llvm-14/cmake                  # Linux
```

### Link errors
Make sure you have the correct LLVM version (14+):
```bash
llvm-config --version
```

### Compilation errors
Ensure you're using a C++20 compatible compiler:
- GCC 10+
- Clang 13+
- MSVC 19.29+

## Getting Help

- Documentation: See `docs/` directory
- Issues: Report bugs via GitHub Issues
- Discussions: Use GitHub Discussions for questions

## License

MIT License - See LICENSE file for details
