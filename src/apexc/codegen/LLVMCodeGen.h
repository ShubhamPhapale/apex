#pragma once

#include "../ast/AST.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>
#include <unordered_map>

namespace apex::codegen {

class LLVMCodeGen {
public:
    LLVMCodeGen(const std::string& module_name);
    
    bool generate(ast::Module* module);
    
    llvm::Module* get_module() { return module_.get(); }
    
    void dump_ir();
    bool emit_object_file(const std::string& filename);
    bool emit_llvm_ir(const std::string& filename);

private:
    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    
    // Symbol tables
    std::unordered_map<std::string, llvm::Value*> named_values_; // SSA values (immutable)
    std::unordered_map<std::string, llvm::AllocaInst*> named_allocas_; // Mutable variables
    std::unordered_map<std::string, llvm::Function*> functions_;
    std::unordered_map<std::string, llvm::StructType*> structs_;
    
    // Code generation
    llvm::Value* codegen_expr(ast::Expr* expr);
    llvm::Value* codegen_stmt(ast::Stmt* stmt);
    llvm::Function* codegen_function(ast::Item* func);
    llvm::StructType* codegen_struct(ast::Item* struct_item);
    
    llvm::Type* codegen_type(ast::Type* type);
    llvm::Type* get_primitive_type(const std::string& name);
};

} // namespace apex::codegen
