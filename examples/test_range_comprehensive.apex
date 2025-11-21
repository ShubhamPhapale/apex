// Test 1: Basic range
fn test_range_basic() -> i32 {
    let mut sum = 0;
    for i in 0..3 {
        sum = sum + i;
    }
    return sum;  // 0 + 1 + 2 = 3
}

// Test 2: Range with offset
fn test_range_offset() -> i32 {
    let mut sum = 0;
    for i in 10..13 {
        sum = sum + i;
    }
    return sum;  // 10 + 11 + 12 = 33
}

// Test 3: Empty range
fn test_range_empty() -> i32 {
    let mut count = 0;
    for i in 5..5 {
        count = count + 1;
    }
    return count;  // Should return 0 (empty range)
}

// Test 4: Larger range
fn test_range_large() -> i32 {
    let mut sum = 0;
    for i in 1..11 {
        sum = sum + i;
    }
    return sum;  // 1+2+3+4+5+6+7+8+9+10 = 55
}

fn main() -> i32 {
    let t1 = test_range_basic();
    let t2 = test_range_offset();
    let t3 = test_range_empty();
    let t4 = test_range_large();
    
    // Verify: 3 + 33 + 0 + 55 = 91
    return t1 + t2 + t3 + t4;
}
