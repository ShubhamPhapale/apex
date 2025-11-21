#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <optional>

namespace apex {

enum class TokenType {
    // Keywords
    KW_FN, KW_LET, KW_MUT, KW_CONST, KW_STATIC,
    KW_IF, KW_ELSE, KW_MATCH, KW_FOR, KW_WHILE, KW_LOOP,
    KW_BREAK, KW_CONTINUE, KW_RETURN,
    KW_STRUCT, KW_ENUM, KW_IMPL, KW_TRAIT, KW_TYPE,
    KW_PUB, KW_MOD, KW_MODULE, KW_IMPORT, KW_EXTERN,
    KW_UNSAFE, KW_DEFER,
    KW_AS, KW_IN,
    KW_TRUE, KW_FALSE, KW_NULL,
    KW_VOID,
    
    // Primitive types
    KW_I8, KW_I16, KW_I32, KW_I64, KW_I128, KW_ISIZE,
    KW_U8, KW_U16, KW_U32, KW_U64, KW_U128, KW_USIZE,
    KW_F32, KW_F64,
    KW_BOOL, KW_CHAR, KW_BYTE,
    
    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    
    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQ, NE, LT, LE, GT, GE,
    AND_AND, OR_OR, NOT,
    AMP, PIPE, CARET, TILDE, SHL, SHR,
    ASSIGN, PLUS_EQ, MINUS_EQ, STAR_EQ, SLASH_EQ, PERCENT_EQ,
    AMP_EQ, PIPE_EQ, CARET_EQ, SHL_EQ, SHR_EQ,
    DOT_DOT, DOT_DOT_EQ,
    ARROW, FAT_ARROW, COLON_COLON, DOT, QUESTION, AT,
    
    // Punctuation
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACKET, RBRACKET,
    COMMA, SEMICOLON, COLON,
    HASH,
    
    // Special
    END_OF_FILE,
    ERROR,
};

struct SourceLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t offset;
    
    SourceLocation() : line(1), column(1), offset(0) {}
    SourceLocation(std::string file, size_t l, size_t c, size_t o)
        : filename(std::move(file)), line(l), column(c), offset(o) {}
};

struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    
    // For literals
    std::optional<std::variant<int64_t, uint64_t, double, std::string>> value;
    
    Token() : type(TokenType::ERROR) {}
    Token(TokenType t, std::string lex, SourceLocation loc)
        : type(t), lexeme(std::move(lex)), location(std::move(loc)) {}
};

const char* token_type_to_string(TokenType type);
bool is_keyword(std::string_view text);
TokenType keyword_to_token_type(std::string_view text);

} // namespace apex
