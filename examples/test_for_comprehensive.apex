// Test 1: Basic for loop
fn test_for_basic() -> i32 {
    let mut sum = 0;
    for i in 0..5 {
        sum = sum + i;
    }
    return sum;  // Should return 10 (0+1+2+3+4)
}

// Test 2: For loop with larger range
fn test_for_range() -> i32 {
    let mut count = 0;
    for i in 5..10 {
        count = count + 1;
    }
    return count;  // Should return 5
}

// Test 3: Nested for loops
fn test_for_nested() -> i32 {
    let mut product = 0;
    for i in 1..4 {
        for j in 1..3 {
            product = product + i * j;
        }
    }
    return product;  // (1*1 + 1*2) + (2*1 + 2*2) + (3*1 + 3*2) = 3 + 6 + 9 = 18
}

// Test 4: For with computations
fn test_for_computation() -> i32 {
    let mut result = 1;
    for i in 1..5 {
        result = result * 2;
    }
    return result;  // Should return 16 (1*2*2*2*2)
}

fn main() -> i32 {
    let t1 = test_for_basic();
    let t2 = test_for_range();
    let t3 = test_for_nested();
    let t4 = test_for_computation();
    
    // Verify: 10 + 5 + 18 + 16 = 49
    return t1 + t2 + t3 + t4;
}
