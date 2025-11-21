# 8. Example Applications

## Example 1: CLI Tool - File Grep Utility

A command-line tool that searches for patterns in files.

```apex
// grep.apex
import std.io;
import std.fs;
import std.env;
import std.collections.Vec;

struct Options {
    pattern: string,
    files: Vec<string>,
    case_sensitive: bool,
    line_numbers: bool,
}

fn parse_args(args: []string) -> Result<Options, string> {
    if args.len < 3 {
        return Err("Usage: grep [options] pattern file...");
    }
    
    let mut opts = Options {
        pattern: "",
        files: Vec.new(),
        case_sensitive: true,
        line_numbers: false,
    };
    
    let mut i = 1;
    while i < args.len {
        let arg = args[i];
        
        if arg == "-i" {
            opts.case_sensitive = false;
        } else if arg == "-n" {
            opts.line_numbers = true;
        } else if opts.pattern.is_empty() {
            opts.pattern = arg;
        } else {
            opts.files.push(arg);
        }
        
        i += 1;
    }
    
    if opts.files.is_empty() {
        return Err("No files specified");
    }
    
    return Ok(opts);
}

fn search_file(path: string, pattern: string, opts: &Options) -> Result<void, string> {
    let file = match fs.File.open(path) {
        Ok(f) => f,
        Err(e) => return Err("Failed to open {}: {}", path, e),
    };
    defer file.close();
    
    let reader = io.BufReader.new(file);
    let lines = reader.lines();
    let mut line_num = 1;
    
    let search_pattern = if opts.case_sensitive {
        pattern
    } else {
        pattern.to_lowercase()
    };
    
    for line_result in lines {
        let line = line_result?;
        
        let search_line = if opts.case_sensitive {
            line
        } else {
            line.to_lowercase()
        };
        
        if search_line.contains(search_pattern) {
            if opts.line_numbers {
                io.println("{}:{}: {}", path, line_num, line);
            } else {
                io.println("{}: {}", path, line);
            }
        }
        
        line_num += 1;
    }
    
    return Ok(void);
}

fn main(args: []string) -> i32 {
    let opts = match parse_args(args) {
        Ok(o) => o,
        Err(msg) => {
            io.eprintln("{}", msg);
            return 1;
        },
    };
    
    for file in opts.files {
        match search_file(file, opts.pattern, &opts) {
            Ok(_) => {},
            Err(msg) => io.eprintln("{}", msg),
        }
    }
    
    return 0;
}
```

**Usage:**
```bash
apexc -O2 -o grep grep.apex
./grep -i -n "error" *.log
```

## Example 2: Basic Web Server - HTTP Server

A simple HTTP server implementation.

```apex
// server.apex
import std.net.{TcpListener, TcpStream};
import std.io;
import std.thread;
import std.collections.HashMap;
import std.string;

struct Request {
    method: string,
    path: string,
    headers: HashMap<string, string>,
}

struct Response {
    status: i32,
    status_text: string,
    headers: HashMap<string, string>,
    body: string,
}

fn parse_request(stream: &mut TcpStream) -> Result<Request, string> {
    let mut buffer: [u8; 4096] = [0; 4096];
    let bytes_read = stream.read(&mut buffer)?;
    
    let request_str = string.from_utf8(&buffer[..bytes_read])?;
    let lines: Vec<string> = request_str.split("\r\n");
    
    if lines.is_empty() {
        return Err("Empty request");
    }
    
    // Parse request line
    let request_line = lines[0];
    let parts: Vec<string> = request_line.split(" ");
    
    if parts.len() < 3 {
        return Err("Invalid request line");
    }
    
    let mut req = Request {
        method: parts[0],
        path: parts[1],
        headers: HashMap.new(),
    };
    
    // Parse headers
    for i in 1..lines.len() {
        let line = lines[i];
        if line.is_empty() {
            break;
        }
        
        if let Some(colon_pos) = line.find(":") {
            let name = line[..colon_pos].trim();
            let value = line[colon_pos + 1..].trim();
            req.headers.insert(name, value);
        }
    }
    
    return Ok(req);
}

fn send_response(stream: &mut TcpStream, response: Response) -> Result<void, string> {
    let status_line = "HTTP/1.1 {} {}\r\n", response.status, response.status_text;
    stream.write(status_line.as_bytes())?;
    
    // Send headers
    response.headers.insert("Content-Length", response.body.len().to_string());
    
    for (name, value) in response.headers {
        let header_line = "{}: {}\r\n", name, value;
        stream.write(header_line.as_bytes())?;
    }
    
    stream.write(b"\r\n")?;
    stream.write(response.body.as_bytes())?;
    
    return Ok(void);
}

fn handle_request(request: &Request) -> Response {
    match request.path {
        "/" => Response {
            status: 200,
            status_text: "OK",
            headers: HashMap.new(),
            body: "<h1>Welcome to Apex Server!</h1>",
        },
        "/api/status" => Response {
            status: 200,
            status_text: "OK",
            headers: {
                let mut h = HashMap.new();
                h.insert("Content-Type", "application/json");
                h
            },
            body: r#"{"status": "ok", "version": "1.0.0"}"#,
        },
        _ => Response {
            status: 404,
            status_text: "Not Found",
            headers: HashMap.new(),
            body: "<h1>404 - Not Found</h1>",
        },
    }
}

fn handle_client(mut stream: TcpStream) {
    let peer = stream.peer_addr().unwrap();
    io.println("Connection from {}", peer);
    
    match parse_request(&mut stream) {
        Ok(request) => {
            io.println("{} {}", request.method, request.path);
            
            let response = handle_request(&request);
            
            match send_response(&mut stream, response) {
                Ok(_) => {},
                Err(e) => io.eprintln("Error sending response: {}", e),
            }
        },
        Err(e) => {
            io.eprintln("Error parsing request: {}", e);
            
            let error_response = Response {
                status: 400,
                status_text: "Bad Request",
                headers: HashMap.new(),
                body: "<h1>400 - Bad Request</h1>",
            };
            
            send_response(&mut stream, error_response).ok();
        },
    }
}

fn main() -> Result<void, string> {
    let addr = "127.0.0.1:8080";
    let listener = TcpListener.bind(addr)?;
    
    io.println("Server listening on http://{}", addr);
    io.println("Press Ctrl+C to stop");
    
    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                thread.spawn(|| handle_client(stream));
            },
            Err(e) => {
                io.eprintln("Connection failed: {}", e);
            },
        }
    }
    
    return Ok(void);
}
```

**Usage:**
```bash
apexc -O2 -o server server.apex
./server
```

## Example 3: Multithreaded Program - Parallel Matrix Multiplication

```apex
// matrix_mult.apex
import std.thread;
import std.sync.Arc;
import std.collections.Vec;
import std.time.Instant;
import std.io;

struct Matrix {
    data: Vec<Vec<f64>>,
    rows: usize,
    cols: usize,
}

impl Matrix {
    fn new(rows: usize, cols: usize) -> Matrix {
        let mut data = Vec.with_capacity(rows);
        for i in 0..rows {
            let mut row = Vec.with_capacity(cols);
            for j in 0..cols {
                row.push(0.0);
            }
            data.push(row);
        }
        
        return Matrix { data, rows, cols };
    }
    
    fn random(rows: usize, cols: usize) -> Matrix {
        let mut m = Matrix.new(rows, cols);
        for i in 0..rows {
            for j in 0..cols {
                m.data[i][j] = (i * cols + j) as f64 / 100.0;
            }
        }
        return m;
    }
    
    fn get(&self, row: usize, col: usize) -> f64 {
        return self.data[row][col];
    }
    
    fn set(&mut self, row: usize, col: usize, value: f64) {
        self.data[row][col] = value;
    }
}

fn multiply_sequential(a: &Matrix, b: &Matrix) -> Matrix {
    assert(a.cols == b.rows, "Invalid matrix dimensions");
    
    let mut result = Matrix.new(a.rows, b.cols);
    
    for i in 0..a.rows {
        for j in 0..b.cols {
            let mut sum = 0.0;
            for k in 0..a.cols {
                sum += a.get(i, k) * b.get(k, j);
            }
            result.set(i, j, sum);
        }
    }
    
    return result;
}

fn multiply_parallel(a: &Matrix, b: &Matrix, num_threads: usize) -> Matrix {
    assert(a.cols == b.rows, "Invalid matrix dimensions");
    
    let result = Arc.new(Mutex.new(Matrix.new(a.rows, b.cols)));
    let a_arc = Arc.new(a.clone());
    let b_arc = Arc.new(b.clone());
    
    let rows_per_thread = (a.rows + num_threads - 1) / num_threads;
    let mut threads = Vec.new();
    
    for t in 0..num_threads {
        let start_row = t * rows_per_thread;
        let end_row = min((t + 1) * rows_per_thread, a.rows);
        
        if start_row >= a.rows {
            break;
        }
        
        let result_clone = result.clone();
        let a_clone = a_arc.clone();
        let b_clone = b_arc.clone();
        
        let thread = thread.spawn(|| {
            for i in start_row..end_row {
                for j in 0..b_clone.cols {
                    let mut sum = 0.0;
                    for k in 0..a_clone.cols {
                        sum += a_clone.get(i, k) * b_clone.get(k, j);
                    }
                    
                    let mut result_guard = result_clone.lock().unwrap();
                    result_guard.set(i, j, sum);
                }
            }
        });
        
        threads.push(thread);
    }
    
    for thread in threads {
        thread.join().unwrap();
    }
    
    let result_guard = result.lock().unwrap();
    return (*result_guard).clone();
}

fn main() -> i32 {
    let size = 500;
    let num_threads = 4;
    
    io.println("Creating {}x{} matrices...", size, size);
    let a = Matrix.random(size, size);
    let b = Matrix.random(size, size);
    
    // Sequential multiplication
    io.println("Sequential multiplication...");
    let start = Instant.now();
    let result_seq = multiply_sequential(&a, &b);
    let sequential_time = start.elapsed();
    io.println("Sequential: {:.3}s", sequential_time.as_secs_f64());
    
    // Parallel multiplication
    io.println("Parallel multiplication ({} threads)...", num_threads);
    let start = Instant.now();
    let result_par = multiply_parallel(&a, &b, num_threads);
    let parallel_time = start.elapsed();
    io.println("Parallel: {:.3}s", parallel_time.as_secs_f64());
    
    let speedup = sequential_time.as_secs_f64() / parallel_time.as_secs_f64();
    io.println("Speedup: {:.2}x", speedup);
    
    return 0;
}
```

## Example 4: Embedded Program - LED Blinker (ARM Cortex-M)

```apex
// blinker.apx - Embedded LED blinker for STM32F4
#![no_std]
#![no_main]

import core.{panic_handler, entry};

// Memory-mapped GPIO registers
const GPIOD_BASE: usize = 0x40020C00;
const RCC_BASE: usize = 0x40023800;

#[repr(C)]
struct GpioRegisters {
    moder: u32,      // Mode register
    otyper: u32,     // Output type register
    ospeedr: u32,    // Output speed register
    pupdr: u32,      // Pull-up/pull-down register
    idr: u32,        // Input data register
    odr: u32,        // Output data register
    bsrr: u32,       // Bit set/reset register
    lckr: u32,       // Configuration lock register
    afrl: u32,       // Alternate function low register
    afrh: u32,       // Alternate function high register
}

#[repr(C)]
struct RccRegisters {
    cr: u32,
    pllcfgr: u32,
    cfgr: u32,
    cir: u32,
    ahb1rstr: u32,
    ahb2rstr: u32,
    ahb3rstr: u32,
    _reserved0: u32,
    apb1rstr: u32,
    apb2rstr: u32,
    _reserved1: [u32; 2],
    ahb1enr: u32,
}

unsafe fn enable_gpio_clock() {
    let rcc = RCC_BASE as *mut RccRegisters;
    (*rcc).ahb1enr |= 1 << 3;  // Enable GPIOD clock
}

unsafe fn init_led() {
    let gpio = GPIOD_BASE as *mut GpioRegisters;
    
    // Configure PD12 as output
    let pin = 12;
    (*gpio).moder &= !(0b11 << (pin * 2));
    (*gpio).moder |= 0b01 << (pin * 2);  // General purpose output mode
    
    // Set output type to push-pull
    (*gpio).otyper &= !(1 << pin);
    
    // Set output speed to high
    (*gpio).ospeedr |= 0b11 << (pin * 2);
    
    // No pull-up/pull-down
    (*gpio).pupdr &= !(0b11 << (pin * 2));
}

unsafe fn led_on() {
    let gpio = GPIOD_BASE as *mut GpioRegisters;
    (*gpio).bsrr = 1 << 12;  // Set bit 12
}

unsafe fn led_off() {
    let gpio = GPIOD_BASE as *mut GpioRegisters;
    (*gpio).bsrr = 1 << (12 + 16);  // Reset bit 12
}

fn delay(count: u32) {
    for _ in 0..count {
        // NOP - compiler should not optimize away
        unsafe { asm!("nop"); }
    }
}

#[entry]
fn main() -> ! {
    unsafe {
        enable_gpio_clock();
        init_led();
        
        loop {
            led_on();
            delay(1000000);
            
            led_off();
            delay(1000000);
        }
    }
}

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
```

**Build configuration (project.apx):**
```apex
project {
    name: "blinker",
    target: "thumbv7em-none-eabihf",
    
    build: {
        no_std: true,
        panic: "abort",
        linker_script: "memory.ld",
        optimization: "size",
    },
}
```

## Example 5: Simple Game Loop

```apex
// game.apx - Simple game with SDL2
import sdl2.{Window, Renderer, Event, EventType};
import std.time.{Instant, Duration};
import std.io;

const SCREEN_WIDTH: i32 = 800;
const SCREEN_HEIGHT: i32 = 600;
const TARGET_FPS: u64 = 60;
const FRAME_TIME: Duration = Duration.from_micros(1_000_000 / TARGET_FPS);

struct Player {
    x: f64,
    y: f64,
    width: f64,
    height: f64,
    velocity_x: f64,
    velocity_y: f64,
}

impl Player {
    fn new(x: f64, y: f64) -> Player {
        return Player {
            x: x,
            y: y,
            width: 50.0,
            height: 50.0,
            velocity_x: 0.0,
            velocity_y: 0.0,
        };
    }
    
    fn update(&mut self, delta_time: f64) {
        self.x += self.velocity_x * delta_time;
        self.y += self.velocity_y * delta_time;
        
        // Keep player on screen
        if self.x < 0.0 {
            self.x = 0.0;
        }
        if self.x + self.width > SCREEN_WIDTH as f64 {
            self.x = SCREEN_WIDTH as f64 - self.width;
        }
        if self.y < 0.0 {
            self.y = 0.0;
        }
        if self.y + self.height > SCREEN_HEIGHT as f64 {
            self.y = SCREEN_HEIGHT as f64 - self.height;
        }
    }
    
    fn render(&self, renderer: &mut Renderer) {
        renderer.set_draw_color(255, 0, 0, 255);
        renderer.fill_rect(
            self.x as i32,
            self.y as i32,
            self.width as i32,
            self.height as i32
        );
    }
}

struct Game {
    window: Window,
    renderer: Renderer,
    player: Player,
    running: bool,
}

impl Game {
    fn new() -> Result<Game, string> {
        sdl2.init()?;
        
        let window = Window.new(
            "Apex Game",
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        )?;
        
        let renderer = Renderer.new(&window)?;
        
        return Ok(Game {
            window: window,
            renderer: renderer,
            player: Player.new(375.0, 275.0),
            running: true,
        });
    }
    
    fn handle_events(&mut self) {
        while let Some(event) = Event.poll() {
            match event.event_type {
                EventType.Quit => {
                    self.running = false;
                },
                EventType.KeyDown => {
                    match event.key {
                        Key.Up => self.player.velocity_y = -200.0,
                        Key.Down => self.player.velocity_y = 200.0,
                        Key.Left => self.player.velocity_x = -200.0,
                        Key.Right => self.player.velocity_x = 200.0,
                        Key.Escape => self.running = false,
                        _ => {},
                    }
                },
                EventType.KeyUp => {
                    match event.key {
                        Key.Up | Key.Down => self.player.velocity_y = 0.0,
                        Key.Left | Key.Right => self.player.velocity_x = 0.0,
                        _ => {},
                    }
                },
                _ => {},
            }
        }
    }
    
    fn update(&mut self, delta_time: f64) {
        self.player.update(delta_time);
    }
    
    fn render(&mut self) {
        // Clear screen (black)
        self.renderer.set_draw_color(0, 0, 0, 255);
        self.renderer.clear();
        
        // Render player
        self.player.render(&mut self.renderer);
        
        // Present
        self.renderer.present();
    }
    
    fn run(&mut self) {
        let mut last_time = Instant.now();
        let mut accumulator = Duration.from_secs(0);
        
        while self.running {
            let current_time = Instant.now();
            let frame_time = current_time.duration_since(last_time);
            last_time = current_time;
            
            accumulator += frame_time;
            
            self.handle_events();
            
            // Fixed time step updates
            while accumulator >= FRAME_TIME {
                self.update(FRAME_TIME.as_secs_f64());
                accumulator -= FRAME_TIME;
            }
            
            self.render();
            
            // Frame rate limiting
            let elapsed = Instant.now().duration_since(current_time);
            if elapsed < FRAME_TIME {
                thread.sleep(FRAME_TIME - elapsed);
            }
        }
    }
}

fn main() -> Result<void, string> {
    let mut game = Game.new()?;
    game.run();
    return Ok(void);
}
```

## Example 6: System Calls - Direct System Call Wrapper

```apex
// syscall.apx - Direct system call interface (Linux x86-64)
import std.io;

// System call numbers (Linux x86-64)
const SYS_READ: i64 = 0;
const SYS_WRITE: i64 = 1;
const SYS_OPEN: i64 = 2;
const SYS_CLOSE: i64 = 3;
const SYS_GETPID: i64 = 39;
const SYS_GETTID: i64 = 186;

unsafe fn syscall0(number: i64) -> i64 {
    let result: i64;
    asm!(
        "syscall",
        in("rax") number,
        out("rax") result,
        lateout("rcx") _,
        lateout("r11") _,
    );
    return result;
}

unsafe fn syscall1(number: i64, arg1: i64) -> i64 {
    let result: i64;
    asm!(
        "syscall",
        in("rax") number,
        in("rdi") arg1,
        out("rax") result,
        lateout("rcx") _,
        lateout("r11") _,
    );
    return result;
}

unsafe fn syscall3(number: i64, arg1: i64, arg2: i64, arg3: i64) -> i64 {
    let result: i64;
    asm!(
        "syscall",
        in("rax") number,
        in("rdi") arg1,
        in("rsi") arg2,
        in("rdx") arg3,
        out("rax") result,
        lateout("rcx") _,
        lateout("r11") _,
    );
    return result;
}

fn sys_write(fd: i32, buf: *const u8, count: usize) -> i64 {
    unsafe {
        return syscall3(SYS_WRITE, fd as i64, buf as i64, count as i64);
    }
}

fn sys_read(fd: i32, buf: *mut u8, count: usize) -> i64 {
    unsafe {
        return syscall3(SYS_READ, fd as i64, buf as i64, count as i64);
    }
}

fn sys_getpid() -> i32 {
    unsafe {
        return syscall0(SYS_GETPID) as i32;
    }
}

fn main() -> i32 {
    let message = "Hello from direct system call!\n";
    let bytes_written = sys_write(1, message.as_ptr(), message.len());
    
    let pid = sys_getpid();
    let pid_msg = "Process ID: {}\n", pid;
    sys_write(1, pid_msg.as_ptr(), pid_msg.len());
    
    return 0;
}
```

## Example 7: C Interop - Using C Libraries

```apex
// c_interop.apx - Using libcurl from Apex
import std.io;
import std.string;

// C library declarations
extern "C" {
    // libcurl functions
    fn curl_easy_init() -> *mut void;
    fn curl_easy_setopt(handle: *mut void, option: i32, ...args) -> i32;
    fn curl_easy_perform(handle: *mut void) -> i32;
    fn curl_easy_cleanup(handle: *mut void);
    fn curl_easy_strerror(code: i32) -> *const char;
    
    // curl options
    const CURLOPT_URL: i32 = 10002;
    const CURLOPT_WRITEFUNCTION: i32 = 20011;
    const CURLOPT_WRITEDATA: i32 = 10001;
}

struct Buffer {
    data: Vec<u8>,
}

extern "C" fn write_callback(
    ptr: *const u8,
    size: usize,
    nmemb: usize,
    userdata: *mut void
) -> usize {
    let total_size = size * nmemb;
    let buffer = unsafe { &mut *(userdata as *mut Buffer) };
    
    unsafe {
        for i in 0..total_size {
            buffer.data.push(*(ptr + i));
        }
    }
    
    return total_size;
}

fn http_get(url: string) -> Result<string, string> {
    unsafe {
        let handle = curl_easy_init();
        if handle == null {
            return Err("Failed to initialize curl");
        }
        defer curl_easy_cleanup(handle);
        
        let mut buffer = Buffer { data: Vec.new() };
        
        // Set URL
        curl_easy_setopt(handle, CURLOPT_URL, url.as_ptr());
        
        // Set callback
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &mut buffer as *mut void);
        
        // Perform request
        let result = curl_easy_perform(handle);
        if result != 0 {
            let error_msg = curl_easy_strerror(result);
            return Err(string.from_cstr(error_msg));
        }
        
        let response = string.from_utf8(&buffer.data)?;
        return Ok(response);
    }
}

fn main() -> Result<void, string> {
    let url = "https://api.github.com/users/apex-lang";
    
    io.println("Fetching {}...", url);
    
    let response = http_get(url)?;
    io.println("Response:\n{}", response);
    
    return Ok(void);
}
```

**Build with C library:**
```bash
apexc -O2 -o http_client c_interop.apx -lcurl
```

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  

**Next**: [09-comparison.md](09-comparison.md) - Language Comparison Matrix
