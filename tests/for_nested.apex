// Test: Nested for loops
// Expected: 18 (sum of i*j for i in 1..4, j in 1..3)
fn main() -> i32 {
    let mut sum = 0;
    
    for i in 1..4 {
        for j in 1..3 {
            sum = sum + i * j;
        }
    }
    
    return sum;
}
