// Combined integration test
fn fibonacci_iterative(n: i32) -> i32 {
    if n < 2 {
        return n;
    }
    
    let mut a = 0;
    let mut b = 1;
    let mut i = 2;
    
    while i < n {
        let sum = a + b;
        a = b;
        b = sum;
        i = i + 1;
    }
    
    return a + b;
}

fn main() -> i32 {
    let fib8 = fibonacci_iterative(8);  // Should be 21
    return fib8;
}
