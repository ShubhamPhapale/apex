# 4. Formal Grammar Specification (EBNF)

This document defines the complete grammar for the Apex programming language using Extended Backus-Naur Form (EBNF).

## Notation

- `::=` - Definition
- `|` - Alternative
- `()` - Grouping
- `[]` - Optional (zero or one)
- `{}` - Repetition (zero or more)
- `"..."` - Literal terminal
- `'...'` - Literal terminal
- `/* ... */` - Comments

## Lexical Grammar

### Tokens

```ebnf
Token ::= Keyword | Identifier | Literal | Operator | Punctuation | Comment

Whitespace ::= " " | "\t" | "\n" | "\r"

Comment ::= LineComment | BlockComment
LineComment ::= "//" { any_char_except_newline } "\n"
BlockComment ::= "/*" { any_char } "*/"
```

### Keywords

```ebnf
Keyword ::= 
    "fn" | "let" | "mut" | "const" | "static" |
    "if" | "else" | "match" | "for" | "while" | "loop" |
    "break" | "continue" | "return" |
    "struct" | "enum" | "impl" | "trait" | "type" |
    "pub" | "mod" | "import" | "extern" |
    "unsafe" | "defer" |
    "as" | "in" |
    "true" | "false" | "null" |
    "void" |
    "i8" | "i16" | "i32" | "i64" | "i128" | "isize" |
    "u8" | "u16" | "u32" | "u64" | "u128" | "usize" |
    "f32" | "f64" |
    "bool" | "char" | "byte"
```

### Identifiers

```ebnf
Identifier ::= IdentifierStart { IdentifierContinue }

IdentifierStart ::= 
    "a"..."z" | "A"..."Z" | "_"

IdentifierContinue ::= 
    IdentifierStart | "0"..."9"
```

### Literals

```ebnf
Literal ::= 
    IntegerLiteral | 
    FloatLiteral | 
    BooleanLiteral | 
    CharLiteral | 
    StringLiteral

IntegerLiteral ::= 
    DecimalLiteral |
    HexLiteral |
    OctalLiteral |
    BinaryLiteral

DecimalLiteral ::= Digit { Digit | "_" } [ IntegerSuffix ]

HexLiteral ::= "0x" HexDigit { HexDigit | "_" } [ IntegerSuffix ]

OctalLiteral ::= "0o" OctalDigit { OctalDigit | "_" } [ IntegerSuffix ]

BinaryLiteral ::= "0b" BinaryDigit { BinaryDigit | "_" } [ IntegerSuffix ]

IntegerSuffix ::= 
    "i8" | "i16" | "i32" | "i64" | "i128" | "isize" |
    "u8" | "u16" | "u32" | "u64" | "u128" | "usize"

FloatLiteral ::= 
    Digit { Digit | "_" } "." Digit { Digit | "_" } [ Exponent ] [ FloatSuffix ] |
    Digit { Digit | "_" } Exponent [ FloatSuffix ]

Exponent ::= ("e" | "E") ["+" | "-"] Digit { Digit | "_" }

FloatSuffix ::= "f32" | "f64"

BooleanLiteral ::= "true" | "false"

CharLiteral ::= "'" ( CharContent | EscapeSequence ) "'"

StringLiteral ::= '"' { StringContent | EscapeSequence } '"'

EscapeSequence ::= 
    "\\" ( "n" | "r" | "t" | "\\" | "'" | '"' | "0" ) |
    "\\x" HexDigit HexDigit |
    "\\u" "{" HexDigit { HexDigit } "}"

Digit ::= "0"..."9"
HexDigit ::= "0"..."9" | "a"..."f" | "A"..."F"
OctalDigit ::= "0"..."7"
BinaryDigit ::= "0" | "1"
```

### Operators

```ebnf
Operator ::=
    ArithmeticOp | ComparisonOp | LogicalOp | BitwiseOp |
    AssignmentOp | RangeOp | OtherOp

ArithmeticOp ::= "+" | "-" | "*" | "/" | "%"

ComparisonOp ::= "==" | "!=" | "<" | "<=" | ">" | ">="

LogicalOp ::= "&&" | "||" | "!"

BitwiseOp ::= "&" | "|" | "^" | "~" | "<<" | ">>"

AssignmentOp ::= 
    "=" | "+=" | "-=" | "*=" | "/=" | "%=" |
    "&=" | "|=" | "^=" | "<<=" | ">>="

RangeOp ::= ".." | "..="

OtherOp ::= "->" | "=>" | "::" | "." | "?" | "@"
```

### Punctuation

```ebnf
Punctuation ::= 
    "{" | "}" | "[" | "]" | "(" | ")" |
    "," | ";" | ":" |
    "#" | "$"
```

## Syntactic Grammar

### Program Structure

```ebnf
Program ::= { Item }

Item ::=
    ModuleDecl |
    ImportDecl |
    FunctionDecl |
    StructDecl |
    EnumDecl |
    ImplBlock |
    TraitDecl |
    TypeAlias |
    ConstDecl |
    StaticDecl

ModuleDecl ::= "module" Path ";"

ImportDecl ::= 
    "import" Path [ ImportItems ] ";"

ImportItems ::=
    "." "{" Identifier { "," Identifier } "}" |
    "as" Identifier |
    "." "*"

Path ::= Identifier { "::" Identifier }
```

### Functions

```ebnf
FunctionDecl ::=
    [ "pub" ] [ "unsafe" ] "fn" Identifier [ GenericParams ]
    "(" [ Parameters ] ")" [ "->" Type ]
    ( Block | ";" )

Parameters ::= Parameter { "," Parameter } [ "," ]

Parameter ::= 
    [ "mut" ] Identifier ":" Type [ "=" Expression ]

GenericParams ::= "<" GenericParam { "," GenericParam } ">"

GenericParam ::= Identifier [ ":" TypeBounds ]

TypeBounds ::= TypeBound { "+" TypeBound }

TypeBound ::= Path
```

### Types

```ebnf
Type ::=
    PrimitiveType |
    PointerType |
    ReferenceType |
    ArrayType |
    SliceType |
    TupleType |
    FunctionType |
    PathType |
    GenericType

PrimitiveType ::=
    "i8" | "i16" | "i32" | "i64" | "i128" | "isize" |
    "u8" | "u16" | "u32" | "u64" | "u128" | "usize" |
    "f32" | "f64" |
    "bool" | "char" | "byte" | "void"

PointerType ::= "*" [ "mut" ] Type

ReferenceType ::= "&" [ "mut" ] Type

ArrayType ::= "[" Type ";" Expression "]"

SliceType ::= "[" "]" Type

TupleType ::= "(" [ Type { "," Type } ] ")"

FunctionType ::= "fn" "(" [ Type { "," Type } ] ")" [ "->" Type ]

PathType ::= Path

GenericType ::= Path "<" Type { "," Type } ">"
```

### Structs

```ebnf
StructDecl ::=
    [ "pub" ] "struct" Identifier [ GenericParams ]
    ( StructBody | TupleStructBody | ";" )

StructBody ::= "{" { StructField } "}"

StructField ::= [ "pub" ] Identifier ":" Type ","

TupleStructBody ::= "(" [ Type { "," Type } ] ")" ";"
```

### Enums

```ebnf
EnumDecl ::=
    [ "pub" ] "enum" Identifier [ GenericParams ]
    "{" { EnumVariant } "}"

EnumVariant ::=
    Identifier [ EnumVariantData ] ","

EnumVariantData ::=
    "(" [ Type { "," Type } ] ")" |
    "{" { StructField } "}"
```

### Implementation Blocks

```ebnf
ImplBlock ::=
    "impl" [ GenericParams ] [ Type "for" ] Type
    "{" { ImplItem } "}"

ImplItem ::= FunctionDecl | ConstDecl | TypeAlias
```

### Traits

```ebnf
TraitDecl ::=
    [ "pub" ] "trait" Identifier [ GenericParams ]
    [ ":" TypeBounds ]
    "{" { TraitItem } "}"

TraitItem ::= FunctionDecl | ConstDecl | TypeAlias
```

### Type Aliases

```ebnf
TypeAlias ::= [ "pub" ] "type" Identifier [ GenericParams ] "=" Type ";"
```

### Constants and Statics

```ebnf
ConstDecl ::= [ "pub" ] "const" Identifier ":" Type "=" Expression ";"

StaticDecl ::= [ "pub" ] "static" [ "mut" ] Identifier ":" Type "=" Expression ";"
```

### Statements

```ebnf
Statement ::=
    LetStatement |
    ExpressionStatement |
    ItemStatement

LetStatement ::=
    "let" [ "mut" ] Pattern [ ":" Type ] [ "=" Expression ] ";"

ExpressionStatement ::=
    Expression ";"

ItemStatement ::= Item
```

### Expressions

```ebnf
Expression ::=
    LiteralExpr |
    PathExpr |
    BlockExpr |
    IfExpr |
    MatchExpr |
    LoopExpr |
    ForExpr |
    WhileExpr |
    ReturnExpr |
    BreakExpr |
    ContinueExpr |
    UnsafeExpr |
    DeferExpr |
    CallExpr |
    MethodCallExpr |
    FieldExpr |
    IndexExpr |
    RangeExpr |
    UnaryExpr |
    BinaryExpr |
    CastExpr |
    TupleExpr |
    ArrayExpr |
    StructExpr |
    LambdaExpr |
    GroupExpr

LiteralExpr ::= Literal

PathExpr ::= Path

BlockExpr ::= "{" { Statement } [ Expression ] "}"

IfExpr ::=
    "if" Expression Block
    [ "else" ( IfExpr | Block ) ]

MatchExpr ::=
    "match" Expression "{"
        { MatchArm }
    "}"

MatchArm ::= Pattern [ "if" Expression ] "=>" ( Expression "," | Block )

LoopExpr ::= [ Label ] "loop" Block

ForExpr ::= [ Label ] "for" Pattern "in" Expression Block

WhileExpr ::= [ Label ] "while" Expression Block

Label ::= "'" Identifier ":"

ReturnExpr ::= "return" [ Expression ]

BreakExpr ::= "break" [ Label ] [ Expression ]

ContinueExpr ::= "continue" [ Label ]

UnsafeExpr ::= "unsafe" Block

DeferExpr ::= "defer" ( Expression ";" | Block )

CallExpr ::= Expression "(" [ Arguments ] ")"

Arguments ::= Expression { "," Expression } [ "," ]

MethodCallExpr ::= Expression "." Identifier "(" [ Arguments ] ")"

FieldExpr ::= Expression "." ( Identifier | IntegerLiteral )

IndexExpr ::= Expression "[" Expression "]"

RangeExpr ::=
    Expression ".." [ Expression ] |
    Expression "..=" Expression |
    ".." Expression

UnaryExpr ::= UnaryOp Expression

UnaryOp ::= "-" | "!" | "*" | "&" [ "mut" ] | "~"

BinaryExpr ::= Expression BinaryOp Expression

BinaryOp ::=
    ArithmeticOp | ComparisonOp | LogicalOp | BitwiseOp | AssignmentOp

CastExpr ::= Expression "as" Type

TupleExpr ::= "(" [ Expression { "," Expression } [ "," ] ] ")"

ArrayExpr ::=
    "[" [ Expression { "," Expression } ] "]" |
    "[" Expression ";" Expression "]"

StructExpr ::=
    Path "{" [ StructExprFields ] "}"

StructExprFields ::=
    StructExprField { "," StructExprField } [ "," ]

StructExprField ::=
    Identifier |
    Identifier ":" Expression

LambdaExpr ::=
    "|" [ Parameters ] "|" [ "->" Type ] ( Expression | Block )

GroupExpr ::= "(" Expression ")"
```

### Patterns

```ebnf
Pattern ::=
    LiteralPattern |
    IdentifierPattern |
    WildcardPattern |
    TuplePattern |
    StructPattern |
    EnumPattern |
    RangePattern |
    RefPattern |
    OrPattern

LiteralPattern ::= Literal

IdentifierPattern ::= [ "mut" ] Identifier

WildcardPattern ::= "_"

TuplePattern ::= "(" [ Pattern { "," Pattern } ] ")"

StructPattern ::=
    Path "{" [ StructPatternFields ] "}"

StructPatternFields ::=
    StructPatternField { "," StructPatternField } [ "," [ ".." ] ]

StructPatternField ::=
    Identifier |
    Identifier ":" Pattern

EnumPattern ::=
    Path [ "(" [ Pattern { "," Pattern } ] ")" | StructPattern ]

RangePattern ::=
    Literal ".." Literal |
    Literal "..=" Literal

RefPattern ::= "&" [ "mut" ] Pattern

OrPattern ::= Pattern "|" Pattern { "|" Pattern }
```

### Extern Declarations

```ebnf
ExternBlock ::=
    "extern" [ StringLiteral ]
    "{" { ExternItem } "}"

ExternItem ::=
    ExternFunctionDecl |
    ExternStaticDecl

ExternFunctionDecl ::=
    "fn" Identifier "(" [ Parameters ] ")" [ "->" Type ] ";"

ExternStaticDecl ::=
    "static" [ "mut" ] Identifier ":" Type ";"
```

### Inline Assembly

```ebnf
AsmExpr ::=
    "asm!" "("
        StringLiteral
        { "," AsmOperand }
    ")"

AsmOperand ::=
    "in" "(" StringLiteral ")" Expression |
    "out" "(" StringLiteral ")" Expression |
    "lateout" "(" StringLiteral ")" Expression |
    "inout" "(" StringLiteral ")" Expression |
    "inlateout" "(" StringLiteral ")" Expression |
    "const" Expression |
    "sym" Path
```

### Attributes

```ebnf
Attribute ::=
    "#" "[" AttributeContent "]"

AttributeContent ::=
    Path [ "(" [ AttributeArgs ] ")" ]

AttributeArgs ::=
    AttributeArg { "," AttributeArg }

AttributeArg ::=
    Identifier [ "=" Expression ]
```

## Operator Precedence

From highest to lowest precedence:

| Level | Operator | Description | Associativity |
|-------|----------|-------------|---------------|
| 1 | `()` `[]` `.` `::` | Grouping, array index, field access, path | Left to right |
| 2 | `f()` `method()` | Function and method calls | Left to right |
| 3 | `!` `~` `-` `*` `&` | Unary operators | Right to left |
| 4 | `as` | Type cast | Left to right |
| 5 | `*` `/` `%` | Multiplication, division, remainder | Left to right |
| 6 | `+` `-` | Addition, subtraction | Left to right |
| 7 | `<<` `>>` | Bit shifts | Left to right |
| 8 | `&` | Bitwise AND | Left to right |
| 9 | `^` | Bitwise XOR | Left to right |
| 10 | `\|` | Bitwise OR | Left to right |
| 11 | `==` `!=` `<` `<=` `>` `>=` | Comparison | Left to right |
| 12 | `&&` | Logical AND | Left to right |
| 13 | `\|\|` | Logical OR | Left to right |
| 14 | `..` `..=` | Range | N/A |
| 15 | `=` `+=` `-=` etc. | Assignment | Right to left |

## Reserved Keywords

The following identifiers are reserved for future use:

```
abstract, async, await, become, box, do, final, macro, override,
priv, try, typeof, unsized, virtual, yield
```

## Contextual Keywords

The following keywords have special meaning only in certain contexts:

```
crate, super, self, Self, union, where
```

## Grammar Notes

### Semicolon Rules

- Statements require semicolons
- Expression statements require semicolons
- The last expression in a block does not require a semicolon (implicit return)
- Item declarations do not require semicolons (except type aliases and extern items)

### Trailing Commas

Trailing commas are allowed (and encouraged) in:
- Function parameters
- Function arguments
- Array literals
- Tuple expressions
- Struct expressions
- Enum variants
- Match arms

### Ambiguity Resolution

**Generic vs. Comparison:**
```apex
// Generic
let v: Vec<i32> = Vec.new();

// Comparison (requires parentheses or spaces)
let is_less = (x < y) > z;
let is_less = x < y && y > z;
```

**Closure vs. OR Pattern:**
```apex
// Closure
let f = |x| x + 1;

// OR pattern in match
match value {
    1 | 2 | 3 => handle_small(),
    _ => handle_large(),
}
```

### Automatic Semicolon Insertion

Apex does **not** use automatic semicolon insertion. All statement-ending semicolons must be explicit.

### Comments and Documentation

```ebnf
DocumentationComment ::=
    "///" { any_char_except_newline } "\n" |
    "/**" { any_char } "*/"

InnerDocComment ::=
    "//!" { any_char_except_newline } "\n" |
    "/*!" { any_char } "*/"
```

**Documentation attributes:**
```apex
/// This is a documentation comment for the following item
/// It supports **markdown** formatting
fn documented_function() { }

//! This is an inner documentation comment
//! It documents the enclosing module or crate
```

---

**Author**: Shubham Phapale  
**Repository**: https://github.com/ShubhamPhapale/apex  

**Next**: [05-memory-model.md](05-memory-model.md) - Memory Model and Safety Rules
