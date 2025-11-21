#!/bin/bash

# Test script for Apex compiler
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

APEXC="./build/src/apexc/apexc"

if [ ! -f "$APEXC" ]; then
    echo -e "${RED}Error: Compiler not found at $APEXC${NC}"
    echo "Please run ./build.sh first"
    exit 1
fi

echo -e "${GREEN}Testing Apex Compiler${NC}"
echo "================================"

test_count=0
pass_count=0

run_test() {
    local name=$1
    local file=$2
    local expected_exit=$3
    
    test_count=$((test_count + 1))
    echo -n "Test $test_count: $name... "
    
    if $APEXC "$file" -o /tmp/test_$test_count.o > /tmp/test_$test_count.log 2>&1; then
        actual_exit=0
    else
        actual_exit=1
    fi
    
    if [ "$actual_exit" -eq "$expected_exit" ]; then
        echo -e "${GREEN}PASS${NC}"
        pass_count=$((pass_count + 1))
    else
        echo -e "${RED}FAIL${NC}"
        echo "Expected exit code $expected_exit, got $actual_exit"
        cat /tmp/test_$test_count.log
    fi
}

# Test tokenization
echo ""
echo "=== Lexer Tests ==="
run_test "Tokenize hello.apx" "examples/hello.apx" 0
run_test "Tokenize fibonacci.apx" "examples/fibonacci.apx" 0
run_test "Tokenize struct.apx" "examples/struct.apx" 0

# Test parsing
echo ""
echo "=== Parser Tests ==="
$APEXC --emit-ast examples/hello.apx > /dev/null 2>&1 && echo -e "${GREEN}Parser test PASS${NC}" || echo -e "${RED}Parser test FAIL${NC}"

# Test code generation
echo ""
echo "=== Code Generation Tests ==="
run_test "Generate LLVM IR for hello.apx" "examples/hello.apx" 0
run_test "Generate object file for fibonacci.apx" "examples/fibonacci.apx" 0

# Summary
echo ""
echo "================================"
echo -e "${GREEN}Tests passed: $pass_count/$test_count${NC}"

if [ "$pass_count" -eq "$test_count" ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed${NC}"
    exit 1
fi
