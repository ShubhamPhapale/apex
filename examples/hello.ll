; ModuleID = 'examples/hello.apex'
source_filename = "examples/hello.apex"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-darwin25.1.0"

define i32 @main() {
entry:
  ret i32 50
}
