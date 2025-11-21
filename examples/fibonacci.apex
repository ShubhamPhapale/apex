// Fibonacci function
fn fib(n: i32) -> i32 {
    if n < 2 {
        return n;
    } else {
        return fib(n - 1) + fib(n - 2);
    }
}

fn main() -> i32 {
    return fib(10);
}
