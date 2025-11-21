// Test 1: Basic while loop
fn test_while_basic() -> i32 {
    let mut count = 0;
    let mut i = 0;
    while i < 5 {
        count = count + 1;
        i = i + 1;
    }
    return count;  // Should return 5
}

// Test 2: Nested while loops
fn test_while_nested() -> i32 {
    let mut sum = 0;
    let mut i = 0;
    while i < 3 {
        let mut j = 0;
        while j < 4 {
            sum = sum + 1;
            j = j + 1;
        }
        i = i + 1;
    }
    return sum;  // Should return 12 (3*4)
}

// Test 3: While with early termination
fn test_while_condition() -> i32 {
    let mut x = 0;
    while x < 100 {
        x = x + 7;
        if x > 20 {
            return x;  // Should return 21
        }
    }
    return 0;
}

fn main() -> i32 {
    let t1 = test_while_basic();
    let t2 = test_while_nested();
    let t3 = test_while_condition();
    
    // Verify results: 5 + 12 + 21 = 38
    return t1 + t2 + t3;
}
