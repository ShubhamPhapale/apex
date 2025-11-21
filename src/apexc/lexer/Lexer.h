#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace apex {

class Lexer {
public:
    explicit Lexer(std::string source, std::string filename = "<input>");
    
    Token next_token();
    std::vector<Token> tokenize_all();
    
    const std::vector<std::string>& get_errors() const { return errors_; }
    bool has_errors() const { return !errors_.empty(); }

private:
    std::string source_;
    std::string filename_;
    size_t current_;
    size_t line_;
    size_t column_;
    size_t line_start_;
    std::vector<std::string> errors_;
    
    SourceLocation current_location() const;
    
    bool is_at_end() const;
    char peek() const;
    char peek_next() const;
    char advance();
    bool match(char expected);
    
    void skip_whitespace();
    void skip_line_comment();
    void skip_block_comment();
    
    Token make_token(TokenType type, std::string lexeme);
    Token make_error_token(const std::string& message);
    
    Token scan_identifier();
    Token scan_number();
    Token scan_string();
    Token scan_char();
    
    bool is_alpha(char c) const;
    bool is_digit(char c) const;
    bool is_alphanum(char c) const;
    
    void add_error(const std::string& message);
};

} // namespace apex
