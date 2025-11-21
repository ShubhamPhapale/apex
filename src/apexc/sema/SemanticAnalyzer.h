#pragma once

#include "../ast/AST.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace apex::sema {

struct Symbol {
    std::string name;
    ast::Type* type;
    bool is_mutable;
    bool is_initialized;
    SourceLocation location;
};

struct Scope {
    Scope* parent{nullptr};
    std::unordered_map<std::string, Symbol> symbols;
    
    Symbol* lookup(const std::string& name);
    bool define(const std::string& name, Symbol symbol);
};

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    
    bool analyze(ast::Module* module);
    
    const std::vector<std::string>& get_errors() const { return errors_; }
    const std::vector<std::string>& get_warnings() const { return warnings_; }
    bool has_errors() const { return !errors_.empty(); }

private:
    Scope* current_scope_;
    std::vector<std::unique_ptr<Scope>> scopes_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // Scope management
    void push_scope();
    void pop_scope();
    
    // Error reporting
    void error(const SourceLocation& loc, const std::string& message);
    void warning(const SourceLocation& loc, const std::string& message);
    
    // Analysis functions
    void analyze_item(ast::Item* item);
    void analyze_function(ast::Item* func);
    void analyze_struct(ast::Item* struct_item);
    void analyze_enum(ast::Item* enum_item);
    
    void analyze_stmt(ast::Stmt* stmt);
    void analyze_expr(ast::Expr* expr);
    void analyze_pattern(ast::Pattern* pattern);
    
    // Type checking
    ast::Type* infer_expr_type(ast::Expr* expr);
    bool types_compatible(ast::Type* t1, ast::Type* t2);
    
    // Name resolution
    Symbol* resolve_name(const std::string& name, const SourceLocation& loc);
};

} // namespace apex::sema
