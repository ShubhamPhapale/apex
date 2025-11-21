// Test: Match with wildcard and literal return
// Expected: 99
fn main() -> i32 {
    return match (42) {
        _ => 99,
    };
}
