#include "Parser.h"
#include <sstream>

namespace apex {

Parser::Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens))
    , current_(0) {}

bool Parser::is_at_end() const {
    return peek().type == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::peek_next() const {
    if (current_ + 1 < tokens_.size()) {
        return tokens_[current_ + 1];
    }
    return tokens_.back();
}

const Token& Parser::previous() const {
    if (current_ > 0) {
        return tokens_[current_ - 1];
    }
    return tokens_[0];
}

Token Parser::advance() {
    if (!is_at_end()) current_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
    return peek().type == type;
}

bool Parser::match(const std::initializer_list<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    error_at(peek(), message);
    return peek();
}

void Parser::error(const std::string& message) {
    error_at(peek(), message);
}

void Parser::error_at(const Token& token, const std::string& message) {
    std::ostringstream oss;
    oss << token.location.filename << ":" << token.location.line << ":" 
        << token.location.column << ": error: " << message;
    errors_.push_back(oss.str());
}

void Parser::synchronize() {
    advance();
    
    while (!is_at_end()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::KW_FN:
            case TokenType::KW_STRUCT:
            case TokenType::KW_ENUM:
            case TokenType::KW_IMPL:
            case TokenType::KW_TRAIT:
            case TokenType::KW_LET:
            case TokenType::KW_RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<ast::Module> Parser::parse_module() {
    auto module = std::make_unique<ast::Module>();
    module->name = "<main>";
    module->location = peek().location;
    
    while (!is_at_end()) {
        try {
            auto item = parse_item();
            if (item) {
                module->items.push_back(std::move(item));
            }
        } catch (...) {
            synchronize();
        }
    }
    
    return module;
}

ast::Visibility Parser::parse_visibility() {
    if (match({TokenType::KW_PUB})) {
        return ast::Visibility::Public;
    }
    return ast::Visibility::Private;
}

std::vector<std::string> Parser::parse_path() {
    std::vector<std::string> path;
    
    path.push_back(consume(TokenType::IDENTIFIER, "Expected identifier").lexeme);
    
    while (match({TokenType::COLON_COLON})) {
        path.push_back(consume(TokenType::IDENTIFIER, "Expected identifier after '::'").lexeme);
    }
    
    return path;
}

std::unique_ptr<ast::Item> Parser::parse_item() {
    auto vis = parse_visibility();
    
    if (match({TokenType::KW_FN})) {
        return parse_function(vis);
    } else if (match({TokenType::KW_STRUCT})) {
        return parse_struct(vis);
    } else if (match({TokenType::KW_ENUM})) {
        return parse_enum(vis);
    } else if (match({TokenType::KW_TRAIT})) {
        return parse_trait(vis);
    } else if (match({TokenType::KW_IMPL})) {
        return parse_impl();
    } else if (match({TokenType::KW_TYPE})) {
        return parse_type_alias(vis);
    } else if (match({TokenType::KW_MOD})) {
        return parse_module(vis);
    } else if (match({TokenType::KW_IMPORT})) {
        return parse_import();
    } else if (match({TokenType::KW_EXTERN})) {
        return parse_extern();
    }
    
    error("Expected item declaration");
    return nullptr;
}

std::unique_ptr<ast::Item> Parser::parse_function(ast::Visibility vis) {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Function, previous().location);
    item->visibility = vis;
    
    item->is_unsafe = match({TokenType::KW_UNSAFE});
    
    item->name = consume(TokenType::IDENTIFIER, "Expected function name").lexeme;
    
    if (check(TokenType::LT)) {
        item->generic_params = parse_generic_params();
    }
    
    consume(TokenType::LPAREN, "Expected '(' after function name");
    item->params = parse_function_params();
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    if (match({TokenType::ARROW})) {
        item->return_type = parse_type();
    } else {
        auto void_type = std::make_unique<ast::Type>(ast::TypeKind::Primitive, peek().location);
        void_type->primitive_name = "void";
        item->return_type = std::move(void_type);
    }
    
    if (check(TokenType::LBRACE)) {
        item->body = parse_block_expr();
    } else {
        consume(TokenType::SEMICOLON, "Expected function body or ';'");
    }
    
    return item;
}

std::vector<ast::FunctionParam> Parser::parse_function_params() {
    std::vector<ast::FunctionParam> params;
    
    if (!check(TokenType::RPAREN)) {
        do {
            ast::FunctionParam param;
            param.location = peek().location;
            param.name = consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme;
            consume(TokenType::COLON, "Expected ':' after parameter name");
            param.type = parse_type();
            params.push_back(std::move(param));
        } while (match({TokenType::COMMA}));
    }
    
    return params;
}

std::vector<ast::GenericParam> Parser::parse_generic_params() {
    std::vector<ast::GenericParam> params;
    
    consume(TokenType::LT, "Expected '<'");
    
    if (!check(TokenType::GT)) {
        do {
            ast::GenericParam param;
            param.location = peek().location;
            param.name = consume(TokenType::IDENTIFIER, "Expected generic parameter name").lexeme;
            
            // TODO: Parse trait bounds (: Trait1 + Trait2)
            
            params.push_back(std::move(param));
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::GT, "Expected '>'");
    
    return params;
}

std::unique_ptr<ast::Item> Parser::parse_struct(ast::Visibility vis) {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Struct, previous().location);
    item->visibility = vis;
    
    item->name = consume(TokenType::IDENTIFIER, "Expected struct name").lexeme;
    
    if (check(TokenType::LT)) {
        item->generic_params = parse_generic_params();
    }
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        item->struct_fields.push_back(parse_struct_field());
        if (!match({TokenType::COMMA})) break;
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return item;
}

ast::StructField Parser::parse_struct_field() {
    ast::StructField field;
    field.location = peek().location;
    field.visibility = parse_visibility();
    field.name = consume(TokenType::IDENTIFIER, "Expected field name").lexeme;
    consume(TokenType::COLON, "Expected ':' after field name");
    field.type = parse_type();
    return field;
}

std::unique_ptr<ast::Item> Parser::parse_enum(ast::Visibility vis) {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Enum, previous().location);
    item->visibility = vis;
    
    item->name = consume(TokenType::IDENTIFIER, "Expected enum name").lexeme;
    
    if (check(TokenType::LT)) {
        item->generic_params = parse_generic_params();
    }
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        item->enum_variants.push_back(parse_enum_variant());
        if (!match({TokenType::COMMA})) break;
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return item;
}

ast::EnumVariant Parser::parse_enum_variant() {
    ast::EnumVariant variant;
    variant.location = peek().location;
    variant.name = consume(TokenType::IDENTIFIER, "Expected variant name").lexeme;
    
    // TODO: Parse tuple or struct variants
    
    return variant;
}

std::unique_ptr<ast::Item> Parser::parse_trait(ast::Visibility vis) {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Trait, previous().location);
    item->visibility = vis;
    
    item->name = consume(TokenType::IDENTIFIER, "Expected trait name").lexeme;
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        auto trait_item = parse_item();
        if (trait_item) {
            item->trait_items.push_back(std::move(trait_item));
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return item;
}

std::unique_ptr<ast::Item> Parser::parse_impl() {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Impl, previous().location);
    
    item->impl_type = parse_type();
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        auto impl_item = parse_item();
        if (impl_item) {
            item->impl_items.push_back(std::move(impl_item));
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return item;
}

std::unique_ptr<ast::Item> Parser::parse_type_alias(ast::Visibility vis) {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::TypeAlias, previous().location);
    item->visibility = vis;
    
    item->name = consume(TokenType::IDENTIFIER, "Expected type alias name").lexeme;
    consume(TokenType::ASSIGN, "Expected '=' in type alias");
    item->aliased_type = parse_type();
    consume(TokenType::SEMICOLON, "Expected ';' after type alias");
    
    return item;
}

std::unique_ptr<ast::Item> Parser::parse_module(ast::Visibility vis) {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Module, previous().location);
    item->visibility = vis;
    
    item->name = consume(TokenType::IDENTIFIER, "Expected module name").lexeme;
    
    if (match({TokenType::SEMICOLON})) {
        // External module
        return item;
    }
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        auto module_item = parse_item();
        if (module_item) {
            item->module_items.push_back(std::move(module_item));
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return item;
}

std::unique_ptr<ast::Item> Parser::parse_import() {
    auto item = std::make_unique<ast::Item>(ast::ItemKind::Import, previous().location);
    
    item->import_path = parse_path();
    
    if (match({TokenType::KW_AS})) {
        item->import_alias = consume(TokenType::IDENTIFIER, "Expected alias name").lexeme;
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after import");
    
    return item;
}

std::unique_ptr<ast::Item> Parser::parse_extern() {
    consume(TokenType::LBRACE, "Expected '{'");
    
    auto item = parse_item();
    if (item && item->kind == ast::ItemKind::Function) {
        item->is_extern = true;
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return item;
}

// Statement parsing
std::unique_ptr<ast::Stmt> Parser::parse_statement() {
    if (match({TokenType::KW_LET})) {
        return parse_let_statement();
    }
    
    // Try to parse as expression
    auto expr = parse_expression();
    auto stmt = std::make_unique<ast::Stmt>(ast::StmtKind::Expr, expr->location);
    stmt->expr = std::move(expr);
    stmt->has_semicolon = match({TokenType::SEMICOLON});
    return stmt;
}

std::unique_ptr<ast::Stmt> Parser::parse_let_statement() {
    auto stmt = std::make_unique<ast::Stmt>(ast::StmtKind::Let, previous().location);
    
    stmt->let_pattern = parse_pattern();
    
    if (match({TokenType::COLON})) {
        stmt->let_type = parse_type();
    }
    
    if (match({TokenType::ASSIGN})) {
        stmt->let_initializer = parse_expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after let statement");
    
    return stmt;
}

// Expression parsing (precedence climbing)
std::unique_ptr<ast::Expr> Parser::parse_expression() {
    return parse_assignment();
}

std::unique_ptr<ast::Expr> Parser::parse_assignment() {
    auto expr = parse_logical_or();
    
    if (match({TokenType::ASSIGN, TokenType::PLUS_EQ, TokenType::MINUS_EQ, 
               TokenType::STAR_EQ, TokenType::SLASH_EQ, TokenType::PERCENT_EQ,
               TokenType::AMP_EQ, TokenType::PIPE_EQ, TokenType::CARET_EQ,
               TokenType::SHL_EQ, TokenType::SHR_EQ})) {
        Token op = previous();
        auto right = parse_assignment();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        
        // Map token to binary op
        switch (op.type) {
            case TokenType::ASSIGN: binary->binary_op = ast::BinaryOp::Assign; break;
            case TokenType::PLUS_EQ: binary->binary_op = ast::BinaryOp::AddAssign; break;
            case TokenType::MINUS_EQ: binary->binary_op = ast::BinaryOp::SubAssign; break;
            case TokenType::STAR_EQ: binary->binary_op = ast::BinaryOp::MulAssign; break;
            case TokenType::SLASH_EQ: binary->binary_op = ast::BinaryOp::DivAssign; break;
            case TokenType::PERCENT_EQ: binary->binary_op = ast::BinaryOp::ModAssign; break;
            default: break;
        }
        
        binary->left = std::move(expr);
        binary->right = std::move(right);
        return binary;
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_logical_or() {
    auto expr = parse_logical_and();
    
    while (match({TokenType::OR_OR})) {
        Token op = previous();
        auto right = parse_logical_and();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = ast::BinaryOp::Or;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_logical_and() {
    auto expr = parse_bitwise_or();
    
    while (match({TokenType::AND_AND})) {
        Token op = previous();
        auto right = parse_bitwise_or();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = ast::BinaryOp::And;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_bitwise_or() {
    auto expr = parse_bitwise_xor();
    
    while (match({TokenType::PIPE})) {
        Token op = previous();
        auto right = parse_bitwise_xor();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = ast::BinaryOp::BitOr;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_bitwise_xor() {
    auto expr = parse_bitwise_and();
    
    while (match({TokenType::CARET})) {
        Token op = previous();
        auto right = parse_bitwise_and();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = ast::BinaryOp::BitXor;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_bitwise_and() {
    auto expr = parse_equality();
    
    while (match({TokenType::AMP})) {
        Token op = previous();
        auto right = parse_equality();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = ast::BinaryOp::BitAnd;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_equality() {
    auto expr = parse_comparison();
    
    while (match({TokenType::EQ, TokenType::NE})) {
        Token op = previous();
        auto right = parse_comparison();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = (op.type == TokenType::EQ) ? ast::BinaryOp::Eq : ast::BinaryOp::Ne;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_comparison() {
    auto expr = parse_shift();
    
    while (match({TokenType::LT, TokenType::LE, TokenType::GT, TokenType::GE})) {
        Token op = previous();
        auto right = parse_shift();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        switch (op.type) {
            case TokenType::LT: binary->binary_op = ast::BinaryOp::Lt; break;
            case TokenType::LE: binary->binary_op = ast::BinaryOp::Le; break;
            case TokenType::GT: binary->binary_op = ast::BinaryOp::Gt; break;
            case TokenType::GE: binary->binary_op = ast::BinaryOp::Ge; break;
            default: break;
        }
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_shift() {
    auto expr = parse_term();
    
    while (match({TokenType::SHL, TokenType::SHR})) {
        Token op = previous();
        auto right = parse_term();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = (op.type == TokenType::SHL) ? ast::BinaryOp::Shl : ast::BinaryOp::Shr;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_term() {
    auto expr = parse_factor();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        auto right = parse_factor();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        binary->binary_op = (op.type == TokenType::PLUS) ? ast::BinaryOp::Add : ast::BinaryOp::Sub;
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_factor() {
    auto expr = parse_unary();
    
    while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
        Token op = previous();
        auto right = parse_unary();
        
        auto binary = std::make_unique<ast::Expr>(ast::ExprKind::Binary, op.location);
        switch (op.type) {
            case TokenType::STAR: binary->binary_op = ast::BinaryOp::Mul; break;
            case TokenType::SLASH: binary->binary_op = ast::BinaryOp::Div; break;
            case TokenType::PERCENT: binary->binary_op = ast::BinaryOp::Mod; break;
            default: break;
        }
        binary->left = std::move(expr);
        binary->right = std::move(right);
        expr = std::move(binary);
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_unary() {
    if (match({TokenType::MINUS, TokenType::NOT, TokenType::TILDE, TokenType::STAR, TokenType::AMP})) {
        Token op = previous();
        auto operand = parse_unary();
        
        auto unary = std::make_unique<ast::Expr>(ast::ExprKind::Unary, op.location);
        switch (op.type) {
            case TokenType::MINUS: unary->unary_op = ast::UnaryOp::Neg; break;
            case TokenType::NOT: unary->unary_op = ast::UnaryOp::Not; break;
            case TokenType::TILDE: unary->unary_op = ast::UnaryOp::BitNot; break;
            case TokenType::STAR: unary->unary_op = ast::UnaryOp::Deref; break;
            case TokenType::AMP:
                if (match({TokenType::KW_MUT})) {
                    unary->unary_op = ast::UnaryOp::AddrOfMut;
                } else {
                    unary->unary_op = ast::UnaryOp::AddrOf;
                }
                break;
            default: break;
        }
        unary->operand = std::move(operand);
        return unary;
    }
    
    return parse_postfix();
}

std::unique_ptr<ast::Expr> Parser::parse_postfix() {
    auto expr = parse_primary();
    
    while (true) {
        if (match({TokenType::LPAREN})) {
            // Function call
            auto call = std::make_unique<ast::Expr>(ast::ExprKind::Call, previous().location);
            call->callee = std::move(expr);
            
            if (!check(TokenType::RPAREN)) {
                do {
                    call->arguments.push_back(parse_expression());
                } while (match({TokenType::COMMA}));
            }
            
            consume(TokenType::RPAREN, "Expected ')' after arguments");
            expr = std::move(call);
        } else if (match({TokenType::LBRACKET})) {
            // Index
            auto index = std::make_unique<ast::Expr>(ast::ExprKind::Index, previous().location);
            index->indexed_expr = std::move(expr);
            index->index_expr = parse_expression();
            consume(TokenType::RBRACKET, "Expected ']' after index");
            expr = std::move(index);
        } else if (match({TokenType::DOT})) {
            // Field access
            auto field = std::make_unique<ast::Expr>(ast::ExprKind::FieldAccess, previous().location);
            field->object = std::move(expr);
            field->field_name = consume(TokenType::IDENTIFIER, "Expected field name").lexeme;
            expr = std::move(field);
        } else if (match({TokenType::KW_AS})) {
            // Cast
            auto cast = std::make_unique<ast::Expr>(ast::ExprKind::Cast, previous().location);
            cast->cast_expr = std::move(expr);
            cast->target_type = parse_type();
            expr = std::move(cast);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<ast::Expr> Parser::parse_primary() {
    // Literals
    if (match({TokenType::INTEGER_LITERAL})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        lit->literal_value = 0; // TODO: Parse actual value
        return lit;
    }
    
    if (match({TokenType::FLOAT_LITERAL})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        lit->literal_value = 0.0; // TODO: Parse actual value
        return lit;
    }
    
    if (match({TokenType::STRING_LITERAL})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        lit->literal_value = previous().lexeme; // TODO: Get actual string value
        return lit;
    }
    
    if (match({TokenType::CHAR_LITERAL})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        lit->literal_value = previous().lexeme;
        return lit;
    }
    
    if (match({TokenType::KW_TRUE})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        lit->literal_value = true;
        return lit;
    }
    
    if (match({TokenType::KW_FALSE})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        lit->literal_value = false;
        return lit;
    }
    
    if (match({TokenType::KW_NULL})) {
        auto lit = std::make_unique<ast::Expr>(ast::ExprKind::Literal, previous().location);
        return lit;
    }
    
    // Identifier or path
    if (check(TokenType::IDENTIFIER)) {
        auto path = parse_path();
        
        // Check for struct literal
        if (check(TokenType::LBRACE)) {
            return parse_struct_literal(path);
        }
        
        // Simple identifier
        auto ident = std::make_unique<ast::Expr>(ast::ExprKind::Identifier, previous().location);
        ident->identifier = path[0]; // TODO: Handle full path
        return ident;
    }
    
    // Parenthesized expression or tuple
    if (match({TokenType::LPAREN})) {
        if (match({TokenType::RPAREN})) {
            // Unit type ()
            auto tuple = std::make_unique<ast::Expr>(ast::ExprKind::Tuple, previous().location);
            return tuple;
        }
        
        auto first = parse_expression();
        
        if (match({TokenType::COMMA})) {
            // Tuple
            auto tuple = std::make_unique<ast::Expr>(ast::ExprKind::Tuple, previous().location);
            tuple->tuple_elements.push_back(std::move(first));
            
            if (!check(TokenType::RPAREN)) {
                do {
                    tuple->tuple_elements.push_back(parse_expression());
                } while (match({TokenType::COMMA}));
            }
            
            consume(TokenType::RPAREN, "Expected ')' after tuple");
            return tuple;
        }
        
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return first;
    }
    
    // Array literal
    if (match({TokenType::LBRACKET})) {
        return parse_array_literal();
    }
    
    // Block expression
    if (match({TokenType::LBRACE})) {
        return parse_block_expr();
    }
    
    // If expression
    if (match({TokenType::KW_IF})) {
        return parse_if_expr();
    }
    
    // Match expression
    if (match({TokenType::KW_MATCH})) {
        return parse_match_expr();
    }
    
    error("Expected expression");
    return nullptr;
}

std::unique_ptr<ast::Expr> Parser::parse_block_expr() {
    auto block = std::make_unique<ast::Expr>(ast::ExprKind::Block, previous().location);
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        // Check if this is the final expression (no semicolon)
        if (!check(TokenType::KW_LET) && !check(TokenType::SEMICOLON)) {
            size_t saved = current_;
            auto expr = parse_expression();
            
            if (check(TokenType::RBRACE)) {
                // Final expression
                block->block_expr = std::move(expr);
                break;
            } else {
                // Not final, revert and parse as statement
                current_ = saved;
            }
        }
        
        auto stmt = parse_statement();
        block->block_stmts.push_back(std::move(stmt));
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return block;
}

std::unique_ptr<ast::Expr> Parser::parse_if_expr() {
    auto if_expr = std::make_unique<ast::Expr>(ast::ExprKind::If, previous().location);
    
    if_expr->condition = parse_expression();
    
    consume(TokenType::LBRACE, "Expected '{' after if condition");
    if_expr->then_branch = parse_block_expr();
    
    if (match({TokenType::KW_ELSE})) {
        if (match({TokenType::KW_IF})) {
            // else if
            if_expr->else_branch = parse_if_expr();
        } else {
            consume(TokenType::LBRACE, "Expected '{' after else");
            if_expr->else_branch = parse_block_expr();
        }
    }
    
    return if_expr;
}

std::unique_ptr<ast::Expr> Parser::parse_match_expr() {
    auto match_expr = std::make_unique<ast::Expr>(ast::ExprKind::Match, previous().location);
    
    match_expr->match_expr = parse_expression();
    
    consume(TokenType::LBRACE, "Expected '{' after match expression");
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        ast::MatchArm arm;
        arm.location = peek().location;
        arm.pattern = parse_pattern();
        
        // TODO: Parse guard (if expr)
        
        consume(TokenType::FAT_ARROW, "Expected '=>' after pattern");
        arm.body = parse_expression();
        
        match_expr->match_arms.push_back(std::move(arm));
        
        if (!match({TokenType::COMMA})) break;
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return match_expr;
}

std::unique_ptr<ast::Expr> Parser::parse_struct_literal(std::vector<std::string> path) {
    auto lit = std::make_unique<ast::Expr>(ast::ExprKind::StructLiteral, peek().location);
    lit->struct_path = std::move(path);
    
    consume(TokenType::LBRACE, "Expected '{'");
    
    if (!check(TokenType::RBRACE)) {
        do {
            ast::FieldInit field;
            field.location = peek().location;
            field.name = consume(TokenType::IDENTIFIER, "Expected field name").lexeme;
            consume(TokenType::COLON, "Expected ':' after field name");
            field.value = parse_expression();
            lit->fields.push_back(std::move(field));
        } while (match({TokenType::COMMA}));
    }
    
    consume(TokenType::RBRACE, "Expected '}'");
    
    return lit;
}

std::unique_ptr<ast::Expr> Parser::parse_array_literal() {
    auto arr = std::make_unique<ast::Expr>(ast::ExprKind::ArrayLiteral, previous().location);
    
    if (match({TokenType::RBRACKET})) {
        // Empty array
        return arr;
    }
    
    auto first = parse_expression();
    
    if (match({TokenType::SEMICOLON})) {
        // Repeat syntax: [value; count]
        arr->repeat_value = std::move(first);
        arr->repeat_count = parse_expression();
    } else {
        // Element list: [a, b, c]
        arr->array_elements.push_back(std::move(first));
        
        while (match({TokenType::COMMA})) {
            if (check(TokenType::RBRACKET)) break;
            arr->array_elements.push_back(parse_expression());
        }
    }
    
    consume(TokenType::RBRACKET, "Expected ']'");
    
    return arr;
}

// Type parsing
std::unique_ptr<ast::Type> Parser::parse_type() {
    return parse_type_primary();
}

std::unique_ptr<ast::Type> Parser::parse_type_primary() {
    // Pointer type: *T or *mut T
    if (match({TokenType::STAR})) {
        auto ptr = std::make_unique<ast::Type>(ast::TypeKind::Pointer, previous().location);
        ptr->is_mutable = match({TokenType::KW_MUT});
        ptr->pointee_type = parse_type();
        return ptr;
    }
    
    // Reference type: &T or &mut T
    if (match({TokenType::AMP})) {
        auto ref = std::make_unique<ast::Type>(ast::TypeKind::Reference, previous().location);
        ref->is_mutable = match({TokenType::KW_MUT});
        ref->pointee_type = parse_type();
        return ref;
    }
    
    // Array type: [T; N]
    if (match({TokenType::LBRACKET})) {
        auto arr = std::make_unique<ast::Type>(ast::TypeKind::Array, previous().location);
        arr->element_type = parse_type();
        
        if (match({TokenType::SEMICOLON})) {
            // Fixed size array
            // TODO: Parse size expression
            arr->array_size = 0;
        }
        
        consume(TokenType::RBRACKET, "Expected ']'");
        return arr;
    }
    
    // Tuple type: (T1, T2, ...)
    if (match({TokenType::LPAREN})) {
        auto tuple = std::make_unique<ast::Type>(ast::TypeKind::Tuple, previous().location);
        
        if (!check(TokenType::RPAREN)) {
            do {
                tuple->tuple_types.push_back(parse_type());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expected ')'");
        return tuple;
    }
    
    // Function type: fn(T1, T2) -> R
    if (match({TokenType::KW_FN})) {
        auto func = std::make_unique<ast::Type>(ast::TypeKind::Function, previous().location);
        
        consume(TokenType::LPAREN, "Expected '('");
        
        if (!check(TokenType::RPAREN)) {
            do {
                func->param_types.push_back(parse_type());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expected ')'");
        
        if (match({TokenType::ARROW})) {
            func->return_type = parse_type();
        }
        
        return func;
    }
    
    // Named type (primitive or user-defined)
    if (check(TokenType::IDENTIFIER) || 
        check(TokenType::KW_I8) || check(TokenType::KW_I16) || check(TokenType::KW_I32) || 
        check(TokenType::KW_I64) || check(TokenType::KW_I128) || check(TokenType::KW_ISIZE) ||
        check(TokenType::KW_U8) || check(TokenType::KW_U16) || check(TokenType::KW_U32) || 
        check(TokenType::KW_U64) || check(TokenType::KW_U128) || check(TokenType::KW_USIZE) ||
        check(TokenType::KW_F32) || check(TokenType::KW_F64) ||
        check(TokenType::KW_BOOL) || check(TokenType::KW_CHAR) || check(TokenType::KW_VOID)) {
        
        auto named = std::make_unique<ast::Type>(ast::TypeKind::Named, peek().location);
        
        // Check if primitive
        if (check(TokenType::IDENTIFIER)) {
            named->path_segments = parse_path();
        } else {
            named->primitive_name = advance().lexeme;
        }
        
        // TODO: Parse generic arguments <T1, T2>
        
        return named;
    }
    
    error("Expected type");
    return nullptr;
}

// Pattern parsing
std::unique_ptr<ast::Pattern> Parser::parse_pattern() {
    return parse_pattern_primary();
}

std::unique_ptr<ast::Pattern> Parser::parse_pattern_primary() {
    // Wildcard: _
    if (match({TokenType::IDENTIFIER})) {
        if (previous().lexeme == "_") {
            return std::make_unique<ast::Pattern>(ast::PatternKind::Wildcard, previous().location);
        }
        
        // Identifier binding
        auto pat = std::make_unique<ast::Pattern>(ast::PatternKind::Identifier, previous().location);
        pat->binding_name = previous().lexeme;
        pat->is_mutable = false; // TODO: Check for mut
        return pat;
    }
    
    // Literal patterns
    if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL, 
               TokenType::STRING_LITERAL, TokenType::CHAR_LITERAL,
               TokenType::KW_TRUE, TokenType::KW_FALSE})) {
        auto pat = std::make_unique<ast::Pattern>(ast::PatternKind::Literal, previous().location);
        // TODO: Set literal value
        return pat;
    }
    
    // Tuple pattern
    if (match({TokenType::LPAREN})) {
        auto pat = std::make_unique<ast::Pattern>(ast::PatternKind::Tuple, previous().location);
        
        if (!check(TokenType::RPAREN)) {
            do {
                pat->tuple_patterns.push_back(parse_pattern());
            } while (match({TokenType::COMMA}));
        }
        
        consume(TokenType::RPAREN, "Expected ')'");
        return pat;
    }
    
    error("Expected pattern");
    return nullptr;
}

} // namespace apex
