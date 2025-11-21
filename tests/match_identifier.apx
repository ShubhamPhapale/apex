// Test: Match expressions with identifier patterns
// Expected: 151 (5 + 20 + 99 + 27)
fn main() -> i32 {
    // Test basic identifier pattern binding
    let result1 = match (5) {
        x => x
    };
    
    // Test with computation
    let result2 = match (10) {
        n => n + n
    };
    
    // Test multiple arms with identifier patterns
    let result3 = match (7) {
        _ => 99
    };
    
    // Test in nested expression
    let result4 = match (3) {
        val => val * val * val
    };
    
    result1 + result2 + result3 + result4
    // 5 + 20 + 99 + 27 = 151
}
