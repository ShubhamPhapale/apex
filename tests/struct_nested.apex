// Test: Nested structs
// Expected: 80 (area of 10x8 rectangle)
struct Vec2 {
    pub x: i32,
    pub y: i32,
}

struct Rectangle {
    pub min: Vec2,
    pub max: Vec2,
}

fn main() -> i32 {
    let min = Vec2 { x: 0, y: 0 };
    let max = Vec2 { x: 10, y: 8 };
    let rect = Rectangle { min: min, max: max };
    
    let width = rect.max.x - rect.min.x;
    let height = rect.max.y - rect.min.y;
    
    return width * height;
}
