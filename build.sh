#!/bin/bash

# Apex Compiler Build Script
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Apex Compiler Build Script${NC}"
echo "================================"

# Check for LLVM
LLVM_CONFIG=""
if command -v llvm-config &> /dev/null; then
    LLVM_CONFIG="llvm-config"
elif [ -f "/opt/homebrew/opt/llvm/bin/llvm-config" ]; then
    LLVM_CONFIG="/opt/homebrew/opt/llvm/bin/llvm-config"
    export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
elif [ -f "/usr/local/opt/llvm/bin/llvm-config" ]; then
    LLVM_CONFIG="/usr/local/opt/llvm/bin/llvm-config"
    export PATH="/usr/local/opt/llvm/bin:$PATH"
else
    echo -e "${RED}Error: llvm-config not found${NC}"
    echo "Please install LLVM 14+ using:"
    echo "  macOS:  brew install llvm"
    echo "  Linux:  sudo apt-get install llvm-14 llvm-14-dev"
    exit 1
fi

LLVM_VERSION=$($LLVM_CONFIG --version | cut -d. -f1)
echo -e "${GREEN}Found LLVM version: $LLVM_VERSION${NC}"

# Create build directory
BUILD_DIR="build"
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Removing existing build directory${NC}"
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo -e "${GREEN}Configuring with CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DAPEX_BUILD_TESTS=ON \
    -DAPEX_BUILD_TOOLS=OFF \
    -DAPEX_BUILD_EXAMPLES=ON

echo -e "${GREEN}Building...${NC}"
cmake --build . -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo -e "${GREEN}Build completed successfully!${NC}"
echo ""
echo "Compiler binary: $BUILD_DIR/src/apexc/apexc"
echo ""
echo "To install system-wide, run:"
echo "  sudo cmake --install $BUILD_DIR"
echo ""
echo "To test the compiler:"
echo "  $BUILD_DIR/src/apexc/apexc ../examples/hello.apx"
