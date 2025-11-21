// Test 1: Basic mutable variable
fn test_mut_basic() -> i32 {
    let mut x = 10;
    x = x + 5;
    x = x * 2;
    return x;  // Should return 30
}

// Test 2: Multiple mutable variables
fn test_mut_multiple() -> i32 {
    let mut a = 1;
    let mut b = 2;
    let mut c = 3;
    a = a + b;
    b = b + c;
    c = c + a;
    return a + b + c;  // 3 + 5 + 6 = 14
}

// Test 3: Immutable and mutable mix
fn test_mut_mixed() -> i32 {
    let x = 10;
    let mut y = 5;
    y = y + x;
    let z = y + x;
    return z;  // 15 + 10 = 25
}

// Test 4: Mutable in nested scope
fn test_mut_scope() -> i32 {
    let mut outer = 10;
    outer = outer + 5;
    
    if outer > 0 {
        let mut inner = 20;
        inner = inner + outer;
        outer = inner;
    }
    
    return outer;  // 20 + 15 = 35
}

fn main() -> i32 {
    let t1 = test_mut_basic();
    let t2 = test_mut_multiple();
    let t3 = test_mut_mixed();
    let t4 = test_mut_scope();
    
    // Verify: 30 + 14 + 25 + 35 = 104
    return t1 + t2 + t3 + t4;
}
