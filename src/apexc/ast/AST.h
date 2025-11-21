#pragma once

#include "../lexer/Token.h"
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <variant>

namespace apex::ast {

// Forward declarations
struct Type;
struct Expr;
struct Stmt;
struct Pattern;
struct Item;

// Type system
enum class TypeKind {
    Primitive, Pointer, Reference, Array, Slice, Tuple, Function, Named, Generic
};

struct Type {
    TypeKind kind;
    SourceLocation location;
    
    // For primitives: i32, u64, f32, bool, void, etc.
    std::optional<std::string> primitive_name;
    
    // For pointers/references
    bool is_mutable{false};
    std::unique_ptr<Type> pointee_type;
    
    // For arrays: [T; N]
    std::unique_ptr<Type> element_type;
    std::optional<size_t> array_size;
    
    // For tuples: (T1, T2, ...)
    std::vector<std::unique_ptr<Type>> tuple_types;
    
    // For functions: fn(T1, T2) -> R
    std::vector<std::unique_ptr<Type>> param_types;
    std::unique_ptr<Type> return_type;
    
    // For named types: MyStruct, std::Vec, etc.
    std::vector<std::string> path_segments;
    std::vector<std::unique_ptr<Type>> generic_args;
    
    Type(TypeKind k, SourceLocation loc) : kind(k), location(std::move(loc)) {}
};

// Expressions
enum class ExprKind {
    Literal, Identifier, Binary, Unary, Call, Index, FieldAccess, Cast,
    StructLiteral, ArrayLiteral, Tuple, Block, If, Match, Range, Return,
    While, For, Break, Continue
};

enum class BinaryOp {
    Add, Sub, Mul, Div, Mod,
    Eq, Ne, Lt, Le, Gt, Ge,
    And, Or,
    BitAnd, BitOr, BitXor, Shl, Shr,
    Assign, AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
    AndAssign, OrAssign, XorAssign, ShlAssign, ShrAssign
};

enum class UnaryOp {
    Neg, Not, BitNot, Deref, AddrOf, AddrOfMut
};

struct FieldInit {
    std::string name;
    std::unique_ptr<Expr> value;
    SourceLocation location;
};

struct MatchArm {
    std::unique_ptr<Pattern> pattern;
    std::optional<std::unique_ptr<Expr>> guard;
    std::unique_ptr<Expr> body;
    SourceLocation location;
};

struct Expr {
    ExprKind kind;
    SourceLocation location;
    std::unique_ptr<Type> type_annotation;
    
    // Literal
    std::optional<std::variant<int64_t, uint64_t, double, std::string, bool>> literal_value;
    
    // Identifier
    std::optional<std::string> identifier;
    
    // Binary operation
    BinaryOp binary_op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    
    // Unary operation
    UnaryOp unary_op;
    std::unique_ptr<Expr> operand;
    
    // Call: func(args)
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;
    
    // Index: array[index]
    std::unique_ptr<Expr> indexed_expr;
    std::unique_ptr<Expr> index_expr;
    
    // Field access: expr.field
    std::unique_ptr<Expr> object;
    std::string field_name;
    
    // Cast: expr as Type
    std::unique_ptr<Expr> cast_expr;
    std::unique_ptr<Type> target_type;
    
    // Struct literal: MyStruct { x: 1, y: 2 }
    std::vector<std::string> struct_path;
    std::vector<FieldInit> fields;
    
    // Array literal: [1, 2, 3] or [0; 10]
    std::vector<std::unique_ptr<Expr>> array_elements;
    std::unique_ptr<Expr> repeat_value;
    std::unique_ptr<Expr> repeat_count;
    
    // Tuple: (1, 2, 3)
    std::vector<std::unique_ptr<Expr>> tuple_elements;
    
    // Block: { stmts; expr }
    std::vector<std::unique_ptr<Stmt>> block_stmts;
    std::unique_ptr<Expr> block_expr;
    
    // If: if cond { then } else { else }
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> then_branch;
    std::unique_ptr<Expr> else_branch;
    
    // Match: match expr { arms }
    std::unique_ptr<Expr> match_expr;
    std::vector<MatchArm> match_arms;
    
    // Range: start..end or start..=end
    std::unique_ptr<Expr> range_start;
    std::unique_ptr<Expr> range_end;
    bool is_inclusive{false};
    
    // Return: return expr
    std::unique_ptr<Expr> return_value;
    
    // While: while cond { body }
    std::unique_ptr<Expr> while_condition;
    std::unique_ptr<Expr> while_body;
    
    // For: for pattern in iterator { body }
    std::unique_ptr<Pattern> for_pattern;
    std::unique_ptr<Expr> for_iterator;
    std::unique_ptr<Expr> for_body;
    
    // Break/Continue: break or continue (with optional label)
    std::optional<std::string> loop_label;
    
    Expr(ExprKind k, SourceLocation loc) : kind(k), location(std::move(loc)) {}
};

// Patterns
enum class PatternKind {
    Wildcard, Identifier, Literal, Tuple, Struct, Enum, Range, Or
};

struct Pattern {
    PatternKind kind;
    SourceLocation location;
    
    // Identifier binding
    std::optional<std::string> binding_name;
    bool is_mutable{false};
    
    // Literal pattern
    std::optional<std::variant<int64_t, uint64_t, double, std::string, bool>> literal_value;
    
    // Tuple pattern
    std::vector<std::unique_ptr<Pattern>> tuple_patterns;
    
    // Struct/enum pattern
    std::vector<std::string> path;
    std::vector<std::pair<std::string, std::unique_ptr<Pattern>>> field_patterns;
    
    // Range pattern
    std::unique_ptr<Pattern> range_start;
    std::unique_ptr<Pattern> range_end;
    bool is_inclusive{false};
    
    // Or pattern
    std::vector<std::unique_ptr<Pattern>> or_patterns;
    
    Pattern(PatternKind k, SourceLocation loc) : kind(k), location(std::move(loc)) {}
};

// Statements
enum class StmtKind {
    Let, Expr, Item
};

struct Stmt {
    StmtKind kind;
    SourceLocation location;
    
    // Let statement
    std::unique_ptr<Pattern> let_pattern;
    std::unique_ptr<Type> let_type;
    std::unique_ptr<Expr> let_initializer;
    
    // Expression statement
    std::unique_ptr<Expr> expr;
    bool has_semicolon{true};
    
    // Item statement (function, struct, etc.)
    std::unique_ptr<Item> item;
    
    Stmt(StmtKind k, SourceLocation loc) : kind(k), location(std::move(loc)) {}
};

// Items (top-level declarations)
enum class ItemKind {
    Function, Struct, Enum, Trait, Impl, TypeAlias, Module, Import, Extern
};

enum class Visibility {
    Private, Public
};

struct FunctionParam {
    std::string name;
    std::unique_ptr<Type> type;
    SourceLocation location;
};

struct StructField {
    Visibility visibility;
    std::string name;
    std::unique_ptr<Type> type;
    SourceLocation location;
};

struct EnumVariant {
    std::string name;
    std::vector<std::unique_ptr<Type>> tuple_fields;
    std::vector<StructField> struct_fields;
    SourceLocation location;
};

struct GenericParam {
    std::string name;
    std::vector<std::vector<std::string>> trait_bounds;
    SourceLocation location;
};

struct Item {
    ItemKind kind;
    Visibility visibility;
    SourceLocation location;
    
    std::string name;
    std::vector<GenericParam> generic_params;
    
    // Function
    std::vector<FunctionParam> params;
    std::unique_ptr<Type> return_type;
    std::unique_ptr<Expr> body;
    bool is_extern{false};
    bool is_unsafe{false};
    
    // Struct
    std::vector<StructField> struct_fields;
    
    // Enum
    std::vector<EnumVariant> enum_variants;
    
    // Trait
    std::vector<std::unique_ptr<Item>> trait_items;
    
    // Impl
    std::unique_ptr<Type> impl_type;
    std::optional<std::vector<std::string>> impl_trait;
    std::vector<std::unique_ptr<Item>> impl_items;
    
    // Type alias
    std::unique_ptr<Type> aliased_type;
    
    // Module
    std::vector<std::unique_ptr<Item>> module_items;
    
    // Import
    std::vector<std::string> import_path;
    std::optional<std::string> import_alias;
    
    Item(ItemKind k, SourceLocation loc) : kind(k), visibility(Visibility::Private), location(std::move(loc)) {}
};

// Module (compilation unit)
struct Module {
    std::string name;
    std::vector<std::unique_ptr<Item>> items;
    SourceLocation location;
};

} // namespace apex::ast
