#pragma once

#include "../lexer/Lexer.h"
#include "../ast/AST.h"
#include <memory>
#include <vector>
#include <string>

namespace apex {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    
    std::unique_ptr<ast::Module> parse_module();
    
    const std::vector<std::string>& get_errors() const { return errors_; }
    bool has_errors() const { return !errors_.empty(); }

private:
    std::vector<Token> tokens_;
    size_t current_;
    std::vector<std::string> errors_;
    
    // Token management
    bool is_at_end() const;
    const Token& peek() const;
    const Token& peek_next() const;
    const Token& previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(const std::initializer_list<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    
    // Error handling
    void error(const std::string& message);
    void error_at(const Token& token, const std::string& message);
    void synchronize();
    
    // Parsing functions
    std::unique_ptr<ast::Item> parse_item();
    std::unique_ptr<ast::Item> parse_function(ast::Visibility vis);
    std::unique_ptr<ast::Item> parse_struct(ast::Visibility vis);
    std::unique_ptr<ast::Item> parse_enum(ast::Visibility vis);
    std::unique_ptr<ast::Item> parse_trait(ast::Visibility vis);
    std::unique_ptr<ast::Item> parse_impl();
    std::unique_ptr<ast::Item> parse_type_alias(ast::Visibility vis);
    std::unique_ptr<ast::Item> parse_module(ast::Visibility vis);
    std::unique_ptr<ast::Item> parse_import();
    std::unique_ptr<ast::Item> parse_extern();
    
    std::vector<ast::GenericParam> parse_generic_params();
    std::vector<ast::FunctionParam> parse_function_params();
    ast::StructField parse_struct_field();
    ast::EnumVariant parse_enum_variant();
    
    std::unique_ptr<ast::Stmt> parse_statement();
    std::unique_ptr<ast::Stmt> parse_let_statement();
    
    std::unique_ptr<ast::Expr> parse_expression();
    std::unique_ptr<ast::Expr> parse_assignment();
    std::unique_ptr<ast::Expr> parse_logical_or();
    std::unique_ptr<ast::Expr> parse_logical_and();
    std::unique_ptr<ast::Expr> parse_bitwise_or();
    std::unique_ptr<ast::Expr> parse_bitwise_xor();
    std::unique_ptr<ast::Expr> parse_bitwise_and();
    std::unique_ptr<ast::Expr> parse_equality();
    std::unique_ptr<ast::Expr> parse_range();
    std::unique_ptr<ast::Expr> parse_comparison();
    std::unique_ptr<ast::Expr> parse_shift();
    std::unique_ptr<ast::Expr> parse_term();
    std::unique_ptr<ast::Expr> parse_factor();
    std::unique_ptr<ast::Expr> parse_unary();
    std::unique_ptr<ast::Expr> parse_postfix();
    std::unique_ptr<ast::Expr> parse_primary();
    
    std::unique_ptr<ast::Expr> parse_if_expr();
    std::unique_ptr<ast::Expr> parse_while_expr();
    std::unique_ptr<ast::Expr> parse_for_expr();
    std::unique_ptr<ast::Expr> parse_match_expr();
    std::unique_ptr<ast::Expr> parse_block_expr();
    std::unique_ptr<ast::Expr> parse_struct_literal(std::vector<std::string> path);
    std::unique_ptr<ast::Expr> parse_array_literal();
    
    std::unique_ptr<ast::Type> parse_type();
    std::unique_ptr<ast::Type> parse_type_primary();
    
    std::unique_ptr<ast::Pattern> parse_pattern();
    std::unique_ptr<ast::Pattern> parse_pattern_primary();
    
    std::vector<std::string> parse_path();
    ast::Visibility parse_visibility();
};

} // namespace apex
