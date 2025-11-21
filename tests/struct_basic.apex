// Test: Basic struct
// Expected: 25 (3² + 4²)
struct Point {
    pub x: i32,
    pub y: i32,
}

fn main() -> i32 {
    let p1 = Point { x: 0, y: 0 };
    let p2 = Point { x: 3, y: 4 };
    
    let dx = p2.x - p1.x;
    let dy = p2.y - p1.y;
    
    return dx * dx + dy * dy;
}
