#!/bin/bash

# Test runner for Apex compiler
# Runs all .apex files in tests/ and verifies exit codes

set -e

APEXC="../build/src/apexc/apexc"
TESTS_DIR="."
FAILED=0
PASSED=0

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "======================================"
echo "Running Apex Compiler Test Suite"
echo "======================================"
echo ""

# Check if compiler exists
if [ ! -f "$APEXC" ]; then
    echo -e "${RED}Error: Compiler not found at $APEXC${NC}"
    echo "Please build the compiler first: ./build.sh"
    exit 1
fi

# Function to extract expected exit code from test file
get_expected_exit_code() {
    local file=$1
    local expected=$(grep -m1 "// Expected:" "$file" | sed 's/.*Expected: \([0-9]*\).*/\1/')
    echo "$expected"
}

# Function to run a single test
run_test() {
    local test_file=$1
    local test_name=$(basename "$test_file" .apex)
    local expected=$(get_expected_exit_code "$test_file")
    
    if [ -z "$expected" ]; then
        echo -e "${YELLOW}⊘ SKIP${NC} $test_name (no expected exit code)"
        return
    fi
    
    # Compile
    if ! $APEXC "$test_file" -o "/tmp/${test_name}.o" &> /dev/null; then
        echo -e "${RED}✗ FAIL${NC} $test_name (compilation failed)"
        FAILED=$((FAILED + 1))
        return
    fi
    
    # Link
    if ! clang "/tmp/${test_name}.o" -o "/tmp/${test_name}" &> /dev/null; then
        echo -e "${RED}✗ FAIL${NC} $test_name (linking failed)"
        FAILED=$((FAILED + 1))
        rm -f "/tmp/${test_name}.o"
        return
    fi
    
    # Run and capture exit code (disable errexit temporarily)
    set +e
    /tmp/${test_name}
    local actual=$?
    set -e
    
    # Clean up
    rm -f "/tmp/${test_name}.o" "/tmp/${test_name}"
    
    # Compare
    if [ "$actual" -eq "$expected" ]; then
        echo -e "${GREEN}✓ PASS${NC} $test_name (exit code: $actual)"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAIL${NC} $test_name (expected: $expected, got: $actual)"
        FAILED=$((FAILED + 1))
    fi
}

# Run all tests
for test_file in $TESTS_DIR/*.apex; do
    if [ -f "$test_file" ]; then
        run_test "$test_file"
    fi
done

# Summary
echo ""
echo "======================================"
echo "Test Summary"
echo "======================================"
echo -e "${GREEN}Passed:${NC} $PASSED"
echo -e "${RED}Failed:${NC} $FAILED"
echo "Total:  $((PASSED + FAILED))"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
