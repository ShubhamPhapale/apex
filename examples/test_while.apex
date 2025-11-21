fn main() -> i32 {
    // Test while loop - count from 0 to 9
    let mut sum = 0;
    let mut i = 0;
    
    while i < 10 {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;  // Should return 45
}
