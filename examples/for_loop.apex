fn main() -> i32 {
    // Test for loop with range - sum from 0 to 9
    let sum = 0;
    
    for i in 0..10 {
        sum = sum + i;
    }
    
    return sum;  // Should return 45 (0+1+2+...+9)
}
