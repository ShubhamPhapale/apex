#include "SemanticAnalyzer.h"
#include <sstream>

namespace apex::sema {

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}

bool Scope::define(const std::string& name, Symbol symbol) {
    if (symbols.find(name) != symbols.end()) {
        return false; // Already defined
    }
    symbols[name] = std::move(symbol);
    return true;
}

SemanticAnalyzer::SemanticAnalyzer()
    : current_scope_(nullptr) {
    // Create global scope
    push_scope();
}

void SemanticAnalyzer::push_scope() {
    auto scope = std::make_unique<Scope>();
    scope->parent = current_scope_;
    current_scope_ = scope.get();
    scopes_.push_back(std::move(scope));
}

void SemanticAnalyzer::pop_scope() {
    if (current_scope_) {
        current_scope_ = current_scope_->parent;
    }
}

void SemanticAnalyzer::error(const SourceLocation& loc, const std::string& message) {
    std::ostringstream oss;
    oss << loc.filename << ":" << loc.line << ":" << loc.column << ": error: " << message;
    errors_.push_back(oss.str());
}

void SemanticAnalyzer::warning(const SourceLocation& loc, const std::string& message) {
    std::ostringstream oss;
    oss << loc.filename << ":" << loc.line << ":" << loc.column << ": warning: " << message;
    warnings_.push_back(oss.str());
}

bool SemanticAnalyzer::analyze(ast::Module* module) {
    if (!module) return false;
    
    // First pass: collect top-level declarations
    for (auto& item : module->items) {
        if (item->kind == ast::ItemKind::Function ||
            item->kind == ast::ItemKind::Struct ||
            item->kind == ast::ItemKind::Enum) {
            
            Symbol symbol;
            symbol.name = item->name;
            symbol.type = nullptr; // TODO: Create type representation
            symbol.is_mutable = false;
            symbol.is_initialized = true;
            symbol.location = item->location;
            
            if (!current_scope_->define(item->name, std::move(symbol))) {
                error(item->location, "Redefinition of '" + item->name + "'");
            }
        }
    }
    
    // Second pass: analyze items
    for (auto& item : module->items) {
        analyze_item(item.get());
    }
    
    return !has_errors();
}

void SemanticAnalyzer::analyze_item(ast::Item* item) {
    if (!item) return;
    
    switch (item->kind) {
        case ast::ItemKind::Function:
            analyze_function(item);
            break;
        case ast::ItemKind::Struct:
            analyze_struct(item);
            break;
        case ast::ItemKind::Enum:
            analyze_enum(item);
            break;
        default:
            // TODO: Handle other item kinds
            break;
    }
}

void SemanticAnalyzer::analyze_function(ast::Item* func) {
    push_scope();
    
    // Add parameters to scope
    for (auto& param : func->params) {
        Symbol symbol;
        symbol.name = param.name;
        symbol.type = param.type.get();
        symbol.is_mutable = false;
        symbol.is_initialized = true;
        symbol.location = param.location;
        
        if (!current_scope_->define(param.name, std::move(symbol))) {
            error(param.location, "Redefinition of parameter '" + param.name + "'");
        }
    }
    
    // Analyze body
    if (func->body) {
        analyze_expr(func->body.get());
    }
    
    pop_scope();
}

void SemanticAnalyzer::analyze_struct(ast::Item* struct_item) {
    // TODO: Check for duplicate fields
    for (size_t i = 0; i < struct_item->struct_fields.size(); i++) {
        for (size_t j = i + 1; j < struct_item->struct_fields.size(); j++) {
            if (struct_item->struct_fields[i].name == struct_item->struct_fields[j].name) {
                error(struct_item->struct_fields[j].location,
                      "Duplicate field '" + struct_item->struct_fields[j].name + "'");
            }
        }
    }
}

void SemanticAnalyzer::analyze_enum(ast::Item* enum_item) {
    // TODO: Check for duplicate variants
    for (size_t i = 0; i < enum_item->enum_variants.size(); i++) {
        for (size_t j = i + 1; j < enum_item->enum_variants.size(); j++) {
            if (enum_item->enum_variants[i].name == enum_item->enum_variants[j].name) {
                error(enum_item->enum_variants[j].location,
                      "Duplicate variant '" + enum_item->enum_variants[j].name + "'");
            }
        }
    }
}

void SemanticAnalyzer::analyze_stmt(ast::Stmt* stmt) {
    if (!stmt) return;
    
    switch (stmt->kind) {
        case ast::StmtKind::Let:
            // Analyze initializer first
            if (stmt->let_initializer) {
                analyze_expr(stmt->let_initializer.get());
            }
            
            // Add variable to scope
            if (stmt->let_pattern) {
                if (stmt->let_pattern->kind == ast::PatternKind::Identifier) {
                    if (!stmt->let_pattern->binding_name) {
                        error(stmt->location, "Let statement identifier pattern has no binding_name");
                    } else if (stmt->let_pattern->binding_name->empty()) {
                        error(stmt->location, "Let statement identifier pattern has empty binding_name");
                    } else {
                        const std::string& name = *stmt->let_pattern->binding_name;
                        Symbol symbol;
                        symbol.name = name;
                        symbol.type = stmt->let_type.get();
                        symbol.is_mutable = stmt->let_pattern->is_mutable;
                        symbol.is_initialized = (stmt->let_initializer != nullptr);
                        symbol.location = stmt->location;
                        
                        if (!current_scope_->define(name, std::move(symbol))) {
                            error(stmt->location, "Redefinition of '" + name + "'");
                        }
                    }
                }
            } else {
                error(stmt->location, "Let statement missing pattern");
            }
            break;
        case ast::StmtKind::Expr:
            if (stmt->expr) {
                analyze_expr(stmt->expr.get());
            }
            break;
        case ast::StmtKind::Item:
            if (stmt->item) {
                analyze_item(stmt->item.get());
            }
            break;
    }
}

void SemanticAnalyzer::analyze_expr(ast::Expr* expr) {
    if (!expr) return;
    
    switch (expr->kind) {
        case ast::ExprKind::Literal:
            // Literals are always valid
            break;
            
        case ast::ExprKind::Identifier:
            if (expr->identifier) {
                resolve_name(*expr->identifier, expr->location);
            }
            break;
            
        case ast::ExprKind::Binary:
            // Handle assignment separately
            if (expr->binary_op == ast::BinaryOp::Assign ||
                expr->binary_op == ast::BinaryOp::AddAssign ||
                expr->binary_op == ast::BinaryOp::SubAssign ||
                expr->binary_op == ast::BinaryOp::MulAssign ||
                expr->binary_op == ast::BinaryOp::DivAssign ||
                expr->binary_op == ast::BinaryOp::ModAssign) {
                // For assignment, left side must be an lvalue (identifier)
                // We don't need to analyze the left side as an expr, just verify it's an identifier
                if (expr->left && expr->left->kind == ast::ExprKind::Identifier) {
                    // Verify the variable exists and is mutable
                    if (expr->left->identifier) {
                        const std::string& var_name = *expr->left->identifier;
                        Symbol* sym = current_scope_->lookup(var_name);
                        if (sym && !sym->is_mutable) {
                            // Error: cannot assign to immutable variable
                            // For now, just continue
                        }
                    }
                }
                // Analyze the right side normally
                analyze_expr(expr->right.get());
            } else {
                // Regular binary operators
                analyze_expr(expr->left.get());
                analyze_expr(expr->right.get());
            }
            // TODO: Type checking
            break;
            
        case ast::ExprKind::Unary:
            analyze_expr(expr->operand.get());
            break;
            
        case ast::ExprKind::Call:
            analyze_expr(expr->callee.get());
            for (auto& arg : expr->arguments) {
                analyze_expr(arg.get());
            }
            break;
            
        case ast::ExprKind::Index:
            analyze_expr(expr->indexed_expr.get());
            analyze_expr(expr->index_expr.get());
            break;
            
        case ast::ExprKind::FieldAccess:
            analyze_expr(expr->object.get());
            // TODO: Check if field exists
            break;
            
        case ast::ExprKind::Cast:
            analyze_expr(expr->cast_expr.get());
            // TODO: Check if cast is valid
            break;
            
        case ast::ExprKind::Block:
            push_scope();
            for (auto& stmt : expr->block_stmts) {
                analyze_stmt(stmt.get());
            }
            if (expr->block_expr) {
                analyze_expr(expr->block_expr.get());
            }
            pop_scope();
            break;
            
        case ast::ExprKind::If:
            analyze_expr(expr->condition.get());
            analyze_expr(expr->then_branch.get());
            if (expr->else_branch) {
                analyze_expr(expr->else_branch.get());
            }
            break;
            
        case ast::ExprKind::Match:
            analyze_expr(expr->match_expr.get());
            for (auto& arm : expr->match_arms) {
                // Create new scope for each match arm
                push_scope();
                
                // Bind pattern variables
                if (arm.pattern && arm.pattern->kind == ast::PatternKind::Identifier) {
                    if (arm.pattern->binding_name) {
                        const std::string& name = *arm.pattern->binding_name;
                        Symbol symbol;
                        symbol.name = name;
                        symbol.type = nullptr; // TODO: Infer from match expression
                        symbol.is_mutable = false; // Pattern bindings are immutable
                        symbol.is_initialized = true;
                        symbol.location = arm.pattern->location;
                        current_scope_->define(name, std::move(symbol));
                    }
                }
                
                analyze_expr(arm.body.get());
                pop_scope();
            }
            break;
            
        case ast::ExprKind::ArrayLiteral:
            for (auto& elem : expr->array_elements) {
                analyze_expr(elem.get());
            }
            if (expr->repeat_value) {
                analyze_expr(expr->repeat_value.get());
                analyze_expr(expr->repeat_count.get());
            }
            break;
            
        case ast::ExprKind::Tuple:
            for (auto& elem : expr->tuple_elements) {
                analyze_expr(elem.get());
            }
            break;
            
        case ast::ExprKind::Return:
            if (expr->return_value) {
                analyze_expr(expr->return_value.get());
            }
            break;
            
        case ast::ExprKind::While:
            analyze_expr(expr->while_condition.get());
            analyze_expr(expr->while_body.get());
            break;
            
        case ast::ExprKind::For:
            analyze_expr(expr->for_iterator.get());
            
            // Create new scope for loop body
            push_scope();
            
            // Add loop variable to scope
            if (expr->for_pattern && 
                expr->for_pattern->kind == ast::PatternKind::Identifier &&
                expr->for_pattern->binding_name) {
                Symbol sym;
                sym.name = *expr->for_pattern->binding_name;
                sym.type = nullptr; // Type inference TODO
                sym.is_mutable = false; // Loop variables are immutable
                sym.is_initialized = true;
                sym.location = expr->for_pattern->location;
                current_scope_->define(sym.name, sym);
            }
            
            analyze_expr(expr->for_body.get());
            pop_scope();
            break;
            
        case ast::ExprKind::Break:
        case ast::ExprKind::Continue:
            // TODO: Verify we're inside a loop
            break;
            
        default:
            // TODO: Handle other expression kinds
            break;
    }
}

void SemanticAnalyzer::analyze_pattern(ast::Pattern* pattern) {
    if (!pattern) return;
    
    // TODO: Implement pattern analysis
}

ast::Type* SemanticAnalyzer::infer_expr_type(ast::Expr* /* expr */) {
    // TODO: Implement type inference
    return nullptr;
}

bool SemanticAnalyzer::types_compatible(ast::Type* /* t1 */, ast::Type* /* t2 */) {
    // TODO: Implement type compatibility checking
    return true;
}

Symbol* SemanticAnalyzer::resolve_name(const std::string& name, const SourceLocation& loc) {
    Symbol* symbol = current_scope_->lookup(name);
    if (!symbol) {
        error(loc, "Undefined identifier '" + name + "'");
        return nullptr;
    }
    return symbol;
}

} // namespace apex::sema
