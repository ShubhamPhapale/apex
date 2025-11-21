fn main() -> i32 {
    let mut a = 0;
    let mut b = 1;
    let mut i = 2;
    let n = 8;
    
    while i < n {
        a = b;
        b = a + b;
        i = i + 1;
    }
    
    return a + b;  // Should be 21 for fib(8)
}
