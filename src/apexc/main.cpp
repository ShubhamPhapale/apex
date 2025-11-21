#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "sema/SemanticAnalyzer.h"
#include "codegen/LLVMCodeGen.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

struct CompilerOptions {
    std::string input_file;
    std::string output_file;
    bool emit_llvm_ir{false};
    bool emit_ast{false};
    bool emit_tokens{false};
    bool verbose{false};
    bool help{false};
};

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <input-file>\n"
              << "\nOptions:\n"
              << "  -o <file>          Write output to <file>\n"
              << "  --emit-llvm        Emit LLVM IR instead of object file\n"
              << "  --emit-ast         Print the AST and exit\n"
              << "  --emit-tokens      Print tokens and exit\n"
              << "  -v, --verbose      Enable verbose output\n"
              << "  -h, --help         Display this help message\n"
              << "\nExamples:\n"
              << "  " << program_name << " hello.apex\n"
              << "  " << program_name << " -o hello.o hello.apex\n"
              << "  " << program_name << " --emit-llvm hello.apex\n";
}

CompilerOptions parse_args(int argc, char** argv) {
    CompilerOptions opts;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            opts.help = true;
            return opts;
        } else if (arg == "-o" && i + 1 < argc) {
            opts.output_file = argv[++i];
        } else if (arg == "--emit-llvm") {
            opts.emit_llvm_ir = true;
        } else if (arg == "--emit-ast") {
            opts.emit_ast = true;
        } else if (arg == "--emit-tokens") {
            opts.emit_tokens = true;
        } else if (arg == "-v" || arg == "--verbose") {
            opts.verbose = true;
        } else if (arg[0] != '-') {
            opts.input_file = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            opts.help = true;
            return opts;
        }
    }
    
    return opts;
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void print_tokens(const std::vector<apex::Token>& tokens) {
    std::cout << "\n=== TOKENS ===\n";
    for (const auto& token : tokens) {
        std::cout << token.location.line << ":" << token.location.column << " "
                  << apex::token_type_to_string(token.type) << " \"" << token.lexeme << "\"\n";
    }
}

void print_ast_expr(const apex::ast::Expr* expr, int indent = 0);
void print_ast_stmt(const apex::ast::Stmt* stmt, int indent = 0);
void print_ast_item(const apex::ast::Item* item, int indent = 0);

void print_indent(int indent) {
    for (int i = 0; i < indent; i++) std::cout << "  ";
}

void print_ast_expr(const apex::ast::Expr* expr, int indent) {
    if (!expr) return;
    
    print_indent(indent);
    switch (expr->kind) {
        case apex::ast::ExprKind::Literal:
            std::cout << "Literal\n";
            break;
        case apex::ast::ExprKind::Identifier:
            std::cout << "Identifier: " << (expr->identifier ? *expr->identifier : "?") << "\n";
            break;
        case apex::ast::ExprKind::Binary:
            std::cout << "Binary\n";
            print_ast_expr(expr->left.get(), indent + 1);
            print_ast_expr(expr->right.get(), indent + 1);
            break;
        case apex::ast::ExprKind::Call:
            std::cout << "Call\n";
            print_ast_expr(expr->callee.get(), indent + 1);
            for (const auto& arg : expr->arguments) {
                print_ast_expr(arg.get(), indent + 1);
            }
            break;
        case apex::ast::ExprKind::Block:
            std::cout << "Block\n";
            for (const auto& stmt : expr->block_stmts) {
                print_ast_stmt(stmt.get(), indent + 1);
            }
            if (expr->block_expr) {
                print_ast_expr(expr->block_expr.get(), indent + 1);
            }
            break;
        default:
            std::cout << "Expr (kind " << static_cast<int>(expr->kind) << ")\n";
    }
}

void print_ast_stmt(const apex::ast::Stmt* stmt, int indent) {
    if (!stmt) return;
    
    print_indent(indent);
    switch (stmt->kind) {
        case apex::ast::StmtKind::Let:
            std::cout << "Let\n";
            break;
        case apex::ast::StmtKind::Expr:
            std::cout << "ExprStmt\n";
            print_ast_expr(stmt->expr.get(), indent + 1);
            break;
        default:
            std::cout << "Stmt\n";
    }
}

void print_ast_item(const apex::ast::Item* item, int indent) {
    if (!item) return;
    
    print_indent(indent);
    switch (item->kind) {
        case apex::ast::ItemKind::Function:
            std::cout << "Function: " << item->name << "\n";
            if (item->body) {
                print_ast_expr(item->body.get(), indent + 1);
            }
            break;
        case apex::ast::ItemKind::Struct:
            std::cout << "Struct: " << item->name << "\n";
            break;
        case apex::ast::ItemKind::Enum:
            std::cout << "Enum: " << item->name << "\n";
            break;
        default:
            std::cout << "Item: " << item->name << "\n";
    }
}

void print_ast(const apex::ast::Module* module) {
    std::cout << "\n=== AST ===\n";
    std::cout << "Module: " << module->name << "\n";
    for (const auto& item : module->items) {
        print_ast_item(item.get(), 1);
    }
}

int main(int argc, char** argv) {
    CompilerOptions opts = parse_args(argc, argv);
    
    if (opts.help || opts.input_file.empty()) {
        print_usage(argv[0]);
        return opts.help ? 0 : 1;
    }
    
    if (opts.verbose) {
        std::cout << "Compiling: " << opts.input_file << std::endl;
    }
    
    // Read source file
    std::string source = read_file(opts.input_file);
    if (source.empty()) {
        return 1;
    }
    
    // Lexical analysis
    apex::Lexer lexer(source, opts.input_file);
    auto tokens = lexer.tokenize_all();
    
    if (lexer.has_errors()) {
        for (const auto& error : lexer.get_errors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }
    
    if (opts.emit_tokens) {
        print_tokens(tokens);
        return 0;
    }
    
    if (opts.verbose) {
        std::cout << "Lexing completed: " << tokens.size() << " tokens\n";
    }
    
    // Parsing
    apex::Parser parser(std::move(tokens));
    auto module = parser.parse_module();
    
    if (parser.has_errors()) {
        for (const auto& error : parser.get_errors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }
    
    if (opts.emit_ast) {
        print_ast(module.get());
        return 0;
    }
    
    if (opts.verbose) {
        std::cout << "Parsing completed\n";
    }
    
    // Semantic analysis
    apex::sema::SemanticAnalyzer analyzer;
    if (!analyzer.analyze(module.get())) {
        for (const auto& error : analyzer.get_errors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }
    
    for (const auto& warning : analyzer.get_warnings()) {
        std::cerr << warning << std::endl;
    }
    
    if (opts.verbose) {
        std::cout << "Semantic analysis completed\n";
    }
    
    // Code generation
    apex::codegen::LLVMCodeGen codegen(opts.input_file);
    if (!codegen.generate(module.get())) {
        std::cerr << "Code generation failed\n";
        return 1;
    }
    
    if (opts.verbose) {
        std::cout << "Code generation completed\n";
        codegen.dump_ir();
    }
    
    // Determine output filename
    std::string output_file = opts.output_file;
    if (output_file.empty()) {
        size_t dot = opts.input_file.find_last_of('.');
        if (dot != std::string::npos) {
            output_file = opts.input_file.substr(0, dot);
        } else {
            output_file = opts.input_file;
        }
        output_file += opts.emit_llvm_ir ? ".ll" : ".o";
    }
    
    // Emit output
    bool success;
    if (opts.emit_llvm_ir) {
        success = codegen.emit_llvm_ir(output_file);
    } else {
        success = codegen.emit_object_file(output_file);
    }
    
    if (!success) {
        std::cerr << "Failed to write output file: " << output_file << std::endl;
        return 1;
    }
    
    if (opts.verbose) {
        std::cout << "Output written to: " << output_file << std::endl;
    }
    
    return 0;
}
