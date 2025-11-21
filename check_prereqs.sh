#!/bin/bash

# Apex Compiler Prerequisites Check
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}Apex Compiler Prerequisites Check${NC}"
echo "===================================="
echo ""

# Check CMake
echo -n "Checking CMake... "
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -1 | awk '{print $3}')
    echo -e "${GREEN}✓ Found version $CMAKE_VERSION${NC}"
else
    echo -e "${RED}✗ Not found${NC}"
    echo "Install with: brew install cmake"
    exit 1
fi

# Check C++ compiler
echo -n "Checking C++ compiler... "
if command -v clang++ &> /dev/null; then
    CXX_VERSION=$(clang++ --version | head -1)
    echo -e "${GREEN}✓ Found $CXX_VERSION${NC}"
    
    # Test if compiler works
    echo -n "Testing C++ compiler... "
    if echo "int main() { return 0; }" | clang++ -x c++ - -o /tmp/test_apex 2>/dev/null; then
        echo -e "${GREEN}✓ Working${NC}"
        rm -f /tmp/test_apex
    else
        echo -e "${RED}✗ Not working${NC}"
        echo "You may need to accept Xcode license: sudo xcodebuild -license accept"
        exit 1
    fi
else
    echo -e "${RED}✗ Not found${NC}"
    echo "Install Xcode command line tools: xcode-select --install"
    exit 1
fi

# Check LLVM
echo -n "Checking LLVM... "
LLVM_CONFIG=""
if command -v llvm-config &> /dev/null; then
    LLVM_CONFIG="llvm-config"
elif [ -f "/opt/homebrew/opt/llvm/bin/llvm-config" ]; then
    LLVM_CONFIG="/opt/homebrew/opt/llvm/bin/llvm-config"
elif [ -f "/usr/local/opt/llvm/bin/llvm-config" ]; then
    LLVM_CONFIG="/usr/local/opt/llvm/bin/llvm-config"
fi

if [ -n "$LLVM_CONFIG" ]; then
    LLVM_VERSION=$($LLVM_CONFIG --version)
    LLVM_MAJOR=$(echo $LLVM_VERSION | cut -d. -f1)
    echo -e "${GREEN}✓ Found version $LLVM_VERSION${NC}"
    
    if [ "$LLVM_MAJOR" -lt 14 ]; then
        echo -e "${YELLOW}⚠ Warning: LLVM 14+ recommended, found $LLVM_MAJOR${NC}"
    fi
else
    echo -e "${RED}✗ Not found${NC}"
    echo "Install with: brew install llvm"
    exit 1
fi

# Check Git
echo -n "Checking Git... "
if command -v git &> /dev/null; then
    GIT_VERSION=$(git --version | awk '{print $3}')
    echo -e "${GREEN}✓ Found version $GIT_VERSION${NC}"
else
    echo -e "${YELLOW}⚠ Not found (optional)${NC}"
fi

echo ""
echo -e "${GREEN}All prerequisites satisfied!${NC}"
echo "Ready to build with: ./build.sh"
