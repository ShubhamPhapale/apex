#include "Lexer.h"
#include <cctype>
#include <sstream>

namespace apex {

Lexer::Lexer(std::string source, std::string filename)
    : source_(std::move(source))
    , filename_(std::move(filename))
    , current_(0)
    , line_(1)
    , column_(1)
    , line_start_(0) {}

SourceLocation Lexer::current_location() const {
    return SourceLocation(filename_, line_, column_, current_);
}

bool Lexer::is_at_end() const {
    return current_ >= source_.length();
}

char Lexer::peek() const {
    if (is_at_end()) return '\0';
    return source_[current_];
}

char Lexer::peek_next() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

char Lexer::advance() {
    if (is_at_end()) return '\0';
    char c = source_[current_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
        line_start_ = current_;
    } else {
        column_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (is_at_end() || source_[current_] != expected) return false;
    advance();
    return true;
}

void Lexer::skip_whitespace() {
    while (!is_at_end()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/' && peek_next() == '/') {
            skip_line_comment();
        } else if (c == '/' && peek_next() == '*') {
            skip_block_comment();
        } else {
            break;
        }
    }
}

void Lexer::skip_line_comment() {
    while (peek() != '\n' && !is_at_end()) {
        advance();
    }
}

void Lexer::skip_block_comment() {
    advance(); // /
    advance(); // *
    
    int depth = 1;
    while (depth > 0 && !is_at_end()) {
        if (peek() == '/' && peek_next() == '*') {
            advance();
            advance();
            depth++;
        } else if (peek() == '*' && peek_next() == '/') {
            advance();
            advance();
            depth--;
        } else {
            advance();
        }
    }
    
    if (depth > 0) {
        add_error("Unterminated block comment");
    }
}

Token Lexer::make_token(TokenType type, std::string lexeme) {
    return Token(type, std::move(lexeme), current_location());
}

Token Lexer::make_error_token(const std::string& message) {
    add_error(message);
    return Token(TokenType::ERROR, message, current_location());
}

void Lexer::add_error(const std::string& message) {
    std::ostringstream oss;
    oss << filename_ << ":" << line_ << ":" << column_ << ": " << message;
    errors_.push_back(oss.str());
}

bool Lexer::is_alpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::is_digit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::is_alphanum(char c) const {
    return is_alpha(c) || is_digit(c);
}

Token Lexer::scan_identifier() {
    size_t start = current_ - 1;
    while (is_alphanum(peek())) {
        advance();
    }
    
    std::string text = source_.substr(start, current_ - start);
    
    if (is_keyword(text)) {
        return make_token(keyword_to_token_type(text), text);
    }
    
    return make_token(TokenType::IDENTIFIER, text);
}

Token Lexer::scan_number() {
    size_t start = current_ - 1;
    bool is_float = false;
    
    // Handle hex, binary, octal
    if (source_[start] == '0' && current_ < source_.length()) {
        char prefix = peek();
        if (prefix == 'x' || prefix == 'X') {
            advance(); // x
            while (std::isxdigit(peek())) advance();
        } else if (prefix == 'b' || prefix == 'B') {
            advance(); // b
            while (peek() == '0' || peek() == '1') advance();
        } else if (prefix == 'o' || prefix == 'O') {
            advance(); // o
            while (peek() >= '0' && peek() <= '7') advance();
        }
    }
    
    // Decimal digits
    while (is_digit(peek())) {
        advance();
    }
    
    // Float
    if (peek() == '.' && is_digit(peek_next())) {
        is_float = true;
        advance(); // .
        while (is_digit(peek())) {
            advance();
        }
    }
    
    // Exponent
    if (peek() == 'e' || peek() == 'E') {
        is_float = true;
        advance();
        if (peek() == '+' || peek() == '-') advance();
        while (is_digit(peek())) advance();
    }
    
    // Type suffix (i32, u64, f32, etc.)
    if (is_alpha(peek())) {
        while (is_alphanum(peek())) advance();
    }
    
    std::string lexeme = source_.substr(start, current_ - start);
    TokenType type = is_float ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL;
    
    return make_token(type, lexeme);
}

Token Lexer::scan_string() {
    size_t start = current_ - 1;
    std::string value;
    
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\\') {
            advance();
            if (is_at_end()) break;
            
            char escaped = advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 'r': value += '\r'; break;
                case 't': value += '\t'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '0': value += '\0'; break;
                default:
                    value += escaped;
                    add_error(std::string("Invalid escape sequence: \\") + escaped);
            }
        } else {
            value += advance();
        }
    }
    
    if (is_at_end()) {
        return make_error_token("Unterminated string literal");
    }
    
    advance(); // closing "
    
    std::string lexeme = source_.substr(start, current_ - start);
    Token token = make_token(TokenType::STRING_LITERAL, lexeme);
    token.value = value;
    return token;
}

Token Lexer::scan_char() {
    size_t start = current_ - 1;
    
    if (is_at_end()) {
        return make_error_token("Unterminated character literal");
    }
    
    char value;
    if (peek() == '\\') {
        advance();
        if (is_at_end()) {
            return make_error_token("Unterminated character literal");
        }
        
        char escaped = advance();
        switch (escaped) {
            case 'n': value = '\n'; break;
            case 'r': value = '\r'; break;
            case 't': value = '\t'; break;
            case '\\': value = '\\'; break;
            case '\'': value = '\''; break;
            case '0': value = '\0'; break;
            default:
                value = escaped;
                add_error(std::string("Invalid escape sequence: \\") + escaped);
        }
    } else {
        value = advance();
    }
    
    if (peek() != '\'') {
        return make_error_token("Character literal must contain exactly one character");
    }
    
    advance(); // closing '
    
    std::string lexeme = source_.substr(start, current_ - start);
    Token token = make_token(TokenType::CHAR_LITERAL, lexeme);
    token.value = std::string(1, value);
    return token;
}

Token Lexer::next_token() {
    skip_whitespace();
    
    if (is_at_end()) {
        return make_token(TokenType::END_OF_FILE, "");
    }
    
    char c = advance();
    
    if (is_alpha(c)) return scan_identifier();
    if (is_digit(c)) return scan_number();
    
    switch (c) {
        case '(': return make_token(TokenType::LPAREN, "(");
        case ')': return make_token(TokenType::RPAREN, ")");
        case '{': return make_token(TokenType::LBRACE, "{");
        case '}': return make_token(TokenType::RBRACE, "}");
        case '[': return make_token(TokenType::LBRACKET, "[");
        case ']': return make_token(TokenType::RBRACKET, "]");
        case ',': return make_token(TokenType::COMMA, ",");
        case ';': return make_token(TokenType::SEMICOLON, ";");
        case '~': return make_token(TokenType::TILDE, "~");
        case '?': return make_token(TokenType::QUESTION, "?");
        case '@': return make_token(TokenType::AT, "@");
        case '#': return make_token(TokenType::HASH, "#");
        
        case ':':
            if (match(':')) return make_token(TokenType::COLON_COLON, "::");
            return make_token(TokenType::COLON, ":");
        
        case '.':
            if (match('.')) {
                if (match('=')) return make_token(TokenType::DOT_DOT_EQ, "..=");
                return make_token(TokenType::DOT_DOT, "..");
            }
            return make_token(TokenType::DOT, ".");
        
        case '+':
            if (match('=')) return make_token(TokenType::PLUS_EQ, "+=");
            return make_token(TokenType::PLUS, "+");
        
        case '-':
            if (match('=')) return make_token(TokenType::MINUS_EQ, "-=");
            if (match('>')) return make_token(TokenType::ARROW, "->");
            return make_token(TokenType::MINUS, "-");
        
        case '*':
            if (match('=')) return make_token(TokenType::STAR_EQ, "*=");
            return make_token(TokenType::STAR, "*");
        
        case '/':
            if (match('=')) return make_token(TokenType::SLASH_EQ, "/=");
            return make_token(TokenType::SLASH, "/");
        
        case '%':
            if (match('=')) return make_token(TokenType::PERCENT_EQ, "%=");
            return make_token(TokenType::PERCENT, "%");
        
        case '&':
            if (match('&')) return make_token(TokenType::AND_AND, "&&");
            if (match('=')) return make_token(TokenType::AMP_EQ, "&=");
            return make_token(TokenType::AMP, "&");
        
        case '|':
            if (match('|')) return make_token(TokenType::OR_OR, "||");
            if (match('=')) return make_token(TokenType::PIPE_EQ, "|=");
            return make_token(TokenType::PIPE, "|");
        
        case '^':
            if (match('=')) return make_token(TokenType::CARET_EQ, "^=");
            return make_token(TokenType::CARET, "^");
        
        case '!':
            if (match('=')) return make_token(TokenType::NE, "!=");
            return make_token(TokenType::NOT, "!");
        
        case '=':
            if (match('=')) return make_token(TokenType::EQ, "==");
            if (match('>')) return make_token(TokenType::FAT_ARROW, "=>");
            return make_token(TokenType::ASSIGN, "=");
        
        case '<':
            if (match('<')) {
                if (match('=')) return make_token(TokenType::SHL_EQ, "<<=");
                return make_token(TokenType::SHL, "<<");
            }
            if (match('=')) return make_token(TokenType::LE, "<=");
            return make_token(TokenType::LT, "<");
        
        case '>':
            if (match('>')) {
                if (match('=')) return make_token(TokenType::SHR_EQ, ">>=");
                return make_token(TokenType::SHR, ">>");
            }
            if (match('=')) return make_token(TokenType::GE, ">=");
            return make_token(TokenType::GT, ">");
        
        case '"':
            return scan_string();
        
        case '\'':
            return scan_char();
        
        default:
            return make_error_token(std::string("Unexpected character: ") + c);
    }
}

std::vector<Token> Lexer::tokenize_all() {
    std::vector<Token> tokens;
    
    while (true) {
        Token token = next_token();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE || token.type == TokenType::ERROR) {
            break;
        }
    }
    
    return tokens;
}

} // namespace apex
