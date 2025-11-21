// Test: Assignment to mutable variables
// Expected: 42 (10 + 15 + 17)
fn main() -> i32 {
    let mut x = 10;
    let mut y = 15;
    let mut z = 5;
    
    z = z + 12;  // z becomes 17
    
    return x + y + z;
}
