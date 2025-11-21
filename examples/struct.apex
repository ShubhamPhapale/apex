// Struct example
struct Point {
    pub x: i32,
    pub y: i32,
}

fn distance_squared(p1: Point, p2: Point) -> i32 {
    let dx: i32 = p1.x - p2.x;
    let dy: i32 = p1.y - p2.y;
    return dx * dx + dy * dy;
}

fn main() -> i32 {
    let p1: Point = Point { x: 0, y: 0 };
    let p2: Point = Point { x: 3, y: 4 };
    return distance_squared(p1, p2);
}
