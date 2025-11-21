// Comprehensive test of all features
fn main() -> i32 {
    // Structs
    let p = Point { x: 3, y: 4 };
    let dist_sq = p.x * p.x + p.y * p.y;  // 25
    
    // Mutable variables and assignment
    let mut result = 0;
    result = result + dist_sq;  // 25
    
    // For loop with range
    for i in 0..5 {
        result = result + i;  // adds 0+1+2+3+4 = 10
    }
    
    return result;  // 35
}

struct Point {
    pub x: i32,
    pub y: i32,
}
