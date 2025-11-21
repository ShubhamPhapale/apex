fn main() -> i32 {
    // Test for loop with mutable accumulator
    let mut sum = 0;
    
    for i in 0..10 {
        sum = sum + i;
    }
    
    return sum;  // Should return 45 (0+1+2+...+9)
}
