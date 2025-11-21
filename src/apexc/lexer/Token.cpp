#include "Token.h"
#include <unordered_map>

namespace apex {

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::KW_FN: return "fn";
        case TokenType::KW_LET: return "let";
        case TokenType::KW_MUT: return "mut";
        case TokenType::KW_CONST: return "const";
        case TokenType::KW_STATIC: return "static";
        case TokenType::KW_IF: return "if";
        case TokenType::KW_ELSE: return "else";
        case TokenType::KW_MATCH: return "match";
        case TokenType::KW_FOR: return "for";
        case TokenType::KW_WHILE: return "while";
        case TokenType::KW_LOOP: return "loop";
        case TokenType::KW_BREAK: return "break";
        case TokenType::KW_CONTINUE: return "continue";
        case TokenType::KW_RETURN: return "return";
        case TokenType::KW_STRUCT: return "struct";
        case TokenType::KW_ENUM: return "enum";
        case TokenType::KW_IMPL: return "impl";
        case TokenType::KW_TRAIT: return "trait";
        case TokenType::KW_TYPE: return "type";
        case TokenType::KW_PUB: return "pub";
        case TokenType::KW_MOD: return "mod";
        case TokenType::KW_MODULE: return "module";
        case TokenType::KW_IMPORT: return "import";
        case TokenType::KW_EXTERN: return "extern";
        case TokenType::KW_UNSAFE: return "unsafe";
        case TokenType::KW_DEFER: return "defer";
        case TokenType::KW_AS: return "as";
        case TokenType::KW_IN: return "in";
        case TokenType::KW_TRUE: return "true";
        case TokenType::KW_FALSE: return "false";
        case TokenType::KW_NULL: return "null";
        case TokenType::KW_VOID: return "void";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::INTEGER_LITERAL: return "integer";
        case TokenType::FLOAT_LITERAL: return "float";
        case TokenType::STRING_LITERAL: return "string";
        case TokenType::CHAR_LITERAL: return "char";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::PERCENT: return "%";
        case TokenType::EQ: return "==";
        case TokenType::NE: return "!=";
        case TokenType::LT: return "<";
        case TokenType::LE: return "<=";
        case TokenType::GT: return ">";
        case TokenType::GE: return ">=";
        case TokenType::AND_AND: return "&&";
        case TokenType::OR_OR: return "||";
        case TokenType::NOT: return "!";
        case TokenType::AMP: return "&";
        case TokenType::PIPE: return "|";
        case TokenType::CARET: return "^";
        case TokenType::TILDE: return "~";
        case TokenType::SHL: return "<<";
        case TokenType::SHR: return ">>";
        case TokenType::ASSIGN: return "=";
        case TokenType::ARROW: return "->";
        case TokenType::FAT_ARROW: return "=>";
        case TokenType::COLON_COLON: return "::";
        case TokenType::DOT: return ".";
        case TokenType::DOT_DOT: return "..";
        case TokenType::DOT_DOT_EQ: return "..=";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::LBRACKET: return "[";
        case TokenType::RBRACKET: return "]";
        case TokenType::COMMA: return ",";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COLON: return ":";
        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

static const std::unordered_map<std::string_view, TokenType> keywords = {
    {"fn", TokenType::KW_FN},
    {"let", TokenType::KW_LET},
    {"mut", TokenType::KW_MUT},
    {"const", TokenType::KW_CONST},
    {"static", TokenType::KW_STATIC},
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"match", TokenType::KW_MATCH},
    {"for", TokenType::KW_FOR},
    {"while", TokenType::KW_WHILE},
    {"loop", TokenType::KW_LOOP},
    {"break", TokenType::KW_BREAK},
    {"continue", TokenType::KW_CONTINUE},
    {"return", TokenType::KW_RETURN},
    {"struct", TokenType::KW_STRUCT},
    {"enum", TokenType::KW_ENUM},
    {"impl", TokenType::KW_IMPL},
    {"trait", TokenType::KW_TRAIT},
    {"type", TokenType::KW_TYPE},
    {"pub", TokenType::KW_PUB},
    {"mod", TokenType::KW_MOD},
    {"module", TokenType::KW_MODULE},
    {"import", TokenType::KW_IMPORT},
    {"extern", TokenType::KW_EXTERN},
    {"unsafe", TokenType::KW_UNSAFE},
    {"defer", TokenType::KW_DEFER},
    {"as", TokenType::KW_AS},
    {"in", TokenType::KW_IN},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"null", TokenType::KW_NULL},
    {"void", TokenType::KW_VOID},
    {"i8", TokenType::KW_I8},
    {"i16", TokenType::KW_I16},
    {"i32", TokenType::KW_I32},
    {"i64", TokenType::KW_I64},
    {"i128", TokenType::KW_I128},
    {"isize", TokenType::KW_ISIZE},
    {"u8", TokenType::KW_U8},
    {"u16", TokenType::KW_U16},
    {"u32", TokenType::KW_U32},
    {"u64", TokenType::KW_U64},
    {"u128", TokenType::KW_U128},
    {"usize", TokenType::KW_USIZE},
    {"f32", TokenType::KW_F32},
    {"f64", TokenType::KW_F64},
    {"bool", TokenType::KW_BOOL},
    {"char", TokenType::KW_CHAR},
    {"byte", TokenType::KW_BYTE},
};

bool is_keyword(std::string_view text) {
    return keywords.find(text) != keywords.end();
}

TokenType keyword_to_token_type(std::string_view text) {
    auto it = keywords.find(text);
    return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
}

} // namespace apex
