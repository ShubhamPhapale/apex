fn count_to_ten() -> i32 {
    let result = 0;
    
    for i in 0..10 {
        result = result + i;
    }
    
    return result;
}

fn main() -> i32 {
    return count_to_ten();  // Should return 45
}
