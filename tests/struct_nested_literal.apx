// Test: Nested struct literals
// Expected: 30 (0 + 10 + 20)
struct Vec2 {
    x: i32,
    y: i32
}

struct Rectangle {
    min: Vec2,
    max: Vec2
}

fn main() -> i32 {
    let rect = Rectangle {
        min: Vec2 { x: 0, y: 10 },
        max: Vec2 { x: 20, y: 30 }
    };
    
    rect.min.x + rect.min.y + rect.max.x
}
