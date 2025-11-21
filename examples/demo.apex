// Demo program showing all working features
// Demonstrates: structs, field access, loops, mutable variables, ranges, assignment

fn main() -> i32 {
    // Feature 1: Structs with field access
    let p1 = Point { x: 3, y: 4 };
    let p2 = Point { x: 5, y: 12 };
    
    // Calculate distances squared
    let dist1 = p1.x * p1.x + p1.y * p1.y;  // 9 + 16 = 25
    let dist2 = p2.x * p2.x + p2.y * p2.y;  // 25 + 144 = 169
    
    // Feature 2: Range expressions with for loops
    let range_sum = sum_range(0, 5);  // 0+1+2+3+4 = 10
    
    // Feature 3: Mutable variables with assignment
    let mut result = dist1;
    result = result + range_sum;  // 25 + 10 = 35
    
    return result;
}

fn sum_range(start: i32, end: i32) -> i32 {
    let mut total = 0;
    let mut i = start;
    while i < end {
        total = total + i;
        i = i + 1;
    }
    return total;
}

struct Point {
    pub x: i32,
    pub y: i32,
}
