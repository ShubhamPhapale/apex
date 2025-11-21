// Test: Mutable variables
// Expected: 30 ((10 + 5) * 2)
fn main() -> i32 {
    let mut x = 10;
    x = x + 5;
    x = x * 2;
    return x;
}
