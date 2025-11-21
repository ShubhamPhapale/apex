#include "LLVMCodeGen.h"
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <iostream>
#include <optional>

namespace apex::codegen {

LLVMCodeGen::LLVMCodeGen(const std::string& module_name) {
    context_ = std::make_unique<llvm::LLVMContext>();
    module_ = std::make_unique<llvm::Module>(module_name, *context_);
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
    
    // Initialize only native target for faster compile times and smaller binary
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    
    // Set up data layout for the native target
    auto target_triple = llvm::sys::getDefaultTargetTriple();
    module_->setTargetTriple(target_triple);
    
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (!target) {
        std::cerr << "Warning: Could not find target: " << error << std::endl;
    } else {
        llvm::TargetOptions opt;
        auto target_machine = target->createTargetMachine(
            target_triple, "generic", "", opt, std::nullopt);
        if (target_machine) {
            module_->setDataLayout(target_machine->createDataLayout());
        }
    }
}

bool LLVMCodeGen::generate(ast::Module* module) {
    if (!module) return false;
    
    // Generate code for all items
    for (auto& item : module->items) {
        switch (item->kind) {
            case ast::ItemKind::Function:
                codegen_function(item.get());
                break;
            case ast::ItemKind::Struct:
                codegen_struct(item.get());
                break;
            default:
                // TODO: Handle other item kinds
                break;
        }
    }
    
    // Verify the module
    std::string error;
    llvm::raw_string_ostream error_stream(error);
    if (llvm::verifyModule(*module_, &error_stream)) {
        std::cerr << "Module verification failed:\n" << error << std::endl;
        return false;
    }
    
    return true;
}

void LLVMCodeGen::dump_ir() {
    module_->print(llvm::outs(), nullptr);
}

bool LLVMCodeGen::emit_llvm_ir(const std::string& filename) {
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        std::cerr << "Could not open file: " << EC.message() << std::endl;
        return false;
    }
    
    module_->print(dest, nullptr);
    return true;
}

bool LLVMCodeGen::emit_object_file(const std::string& filename) {
    auto target_triple = llvm::sys::getDefaultTargetTriple();
    module_->setTargetTriple(target_triple);
    
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(target_triple, error);
    
    if (!target) {
        std::cerr << "Failed to lookup target: " << error << std::endl;
        return false;
    }
    
    auto CPU = "generic";
    auto features = "";
    
    llvm::TargetOptions opt;
    auto RM = std::optional<llvm::Reloc::Model>();
    auto target_machine = target->createTargetMachine(target_triple, CPU, features, opt, RM);
    
    module_->setDataLayout(target_machine->createDataLayout());
    
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename, EC, llvm::sys::fs::OF_None);
    
    if (EC) {
        std::cerr << "Could not open file: " << EC.message() << std::endl;
        return false;
    }
    
    llvm::legacy::PassManager pass;
    auto file_type = llvm::CodeGenFileType::ObjectFile;
    
    if (target_machine->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
        std::cerr << "TargetMachine can't emit a file of this type" << std::endl;
        return false;
    }
    
    pass.run(*module_);
    dest.flush();
    
    return true;
}

llvm::Type* LLVMCodeGen::get_primitive_type(const std::string& name) {
    if (name == "void") return llvm::Type::getVoidTy(*context_);
    if (name == "bool") return llvm::Type::getInt1Ty(*context_);
    if (name == "i8" || name == "u8" || name == "byte") return llvm::Type::getInt8Ty(*context_);
    if (name == "i16" || name == "u16") return llvm::Type::getInt16Ty(*context_);
    if (name == "i32" || name == "u32") return llvm::Type::getInt32Ty(*context_);
    if (name == "i64" || name == "u64") return llvm::Type::getInt64Ty(*context_);
    if (name == "i128" || name == "u128") return llvm::Type::getInt128Ty(*context_);
    if (name == "isize" || name == "usize") return llvm::Type::getInt64Ty(*context_); // Assume 64-bit
    if (name == "f32") return llvm::Type::getFloatTy(*context_);
    if (name == "f64") return llvm::Type::getDoubleTy(*context_);
    if (name == "char") return llvm::Type::getInt32Ty(*context_);
    
    return nullptr;
}

llvm::Type* LLVMCodeGen::codegen_type(ast::Type* type) {
    if (!type) return llvm::Type::getVoidTy(*context_);
    
    switch (type->kind) {
        case ast::TypeKind::Primitive:
            if (type->primitive_name) {
                return get_primitive_type(*type->primitive_name);
            }
            break;
            
        case ast::TypeKind::Pointer:
            if (type->pointee_type) {
                auto pointee = codegen_type(type->pointee_type.get());
                return llvm::PointerType::get(pointee, 0);
            }
            break;
            
        case ast::TypeKind::Array:
            if (type->element_type && type->array_size) {
                auto elem = codegen_type(type->element_type.get());
                return llvm::ArrayType::get(elem, *type->array_size);
            }
            break;
            
        case ast::TypeKind::Named:
            // Check if it's a primitive type name
            if (type->primitive_name) {
                return get_primitive_type(*type->primitive_name);
            }
            // Otherwise look up user-defined types
            if (!type->path_segments.empty()) {
                auto it = structs_.find(type->path_segments[0]);
                if (it != structs_.end()) {
                    return it->second;
                }
            }
            break;
            
        default:
            // TODO: Handle other type kinds
            break;
    }
    
    return llvm::Type::getVoidTy(*context_);
}

llvm::Function* LLVMCodeGen::codegen_function(ast::Item* func) {
    // Create function type
    std::vector<llvm::Type*> param_types;
    for (auto& param : func->params) {
        param_types.push_back(codegen_type(param.type.get()));
    }
    
    llvm::Type* return_type = codegen_type(func->return_type.get());
    llvm::FunctionType* func_type = llvm::FunctionType::get(return_type, param_types, false);
    
    // Create function
    llvm::Function* llvm_func = llvm::Function::Create(
        func_type,
        llvm::Function::ExternalLinkage,
        func->name,
        module_.get()
    );
    
    functions_[func->name] = llvm_func;
    
    // Set parameter names
    size_t idx = 0;
    for (auto& arg : llvm_func->args()) {
        arg.setName(func->params[idx].name);
        idx++;
    }
    
    // Generate function body
    if (func->body) {
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context_, "entry", llvm_func);
        builder_->SetInsertPoint(entry);
        
        // Add parameters to symbol table
        named_values_.clear();
        for (auto& arg : llvm_func->args()) {
            named_values_[std::string(arg.getName())] = &arg;
        }
        
        llvm::Value* ret_val = codegen_expr(func->body.get());
        
        // Only add return if block doesn't already have a terminator
        if (!builder_->GetInsertBlock()->getTerminator()) {
            if (ret_val) {
                if (return_type->isVoidTy()) {
                    builder_->CreateRetVoid();
                } else {
                    builder_->CreateRet(ret_val);
                }
            } else if (return_type->isVoidTy()) {
                builder_->CreateRetVoid();
            }
        }
        
        // Verify function
        std::string error;
        llvm::raw_string_ostream error_stream(error);
        if (llvm::verifyFunction(*llvm_func, &error_stream)) {
            std::cerr << "Function verification failed: " << error << std::endl;
            llvm_func->eraseFromParent();
            return nullptr;
        }
    }
    
    return llvm_func;
}

llvm::StructType* LLVMCodeGen::codegen_struct(ast::Item* struct_item) {
    std::vector<llvm::Type*> field_types;
    
    for (auto& field : struct_item->struct_fields) {
        field_types.push_back(codegen_type(field.type.get()));
    }
    
    llvm::StructType* struct_type = llvm::StructType::create(*context_, field_types, struct_item->name);
    structs_[struct_item->name] = struct_type;
    
    return struct_type;
}

llvm::Value* LLVMCodeGen::codegen_expr(ast::Expr* expr) {
    if (!expr) return nullptr;
    
    switch (expr->kind) {
        case ast::ExprKind::Literal:
            if (expr->literal_value) {
                if (std::holds_alternative<int64_t>(*expr->literal_value)) {
                    int64_t val = std::get<int64_t>(*expr->literal_value);
                    // Default to 32-bit integers for now (TODO: use type inference)
                    return llvm::ConstantInt::get(*context_, llvm::APInt(32, val, true));
                } else if (std::holds_alternative<double>(*expr->literal_value)) {
                    return llvm::ConstantFP::get(*context_, llvm::APFloat(std::get<double>(*expr->literal_value)));
                } else if (std::holds_alternative<bool>(*expr->literal_value)) {
                    return llvm::ConstantInt::get(*context_, llvm::APInt(1, std::get<bool>(*expr->literal_value)));
                }
            }
            return llvm::ConstantInt::get(*context_, llvm::APInt(32, 0));
            
        case ast::ExprKind::Identifier:
            if (expr->identifier) {
                // Check local variables first
                auto it = named_values_.find(*expr->identifier);
                if (it != named_values_.end()) {
                    llvm::Value* val = it->second;
                    // If it's an alloca (local variable), load it
                    if (llvm::AllocaInst* alloca = llvm::dyn_cast<llvm::AllocaInst>(val)) {
                        return builder_->CreateLoad(alloca->getAllocatedType(), val, expr->identifier->c_str());
                    }
                    return val;
                }
                // Check functions
                auto func_it = functions_.find(*expr->identifier);
                if (func_it != functions_.end()) {
                    return func_it->second;
                }
            }
            return nullptr;
            
        case ast::ExprKind::Binary: {
            llvm::Value* left = codegen_expr(expr->left.get());
            llvm::Value* right = codegen_expr(expr->right.get());
            
            if (!left || !right) return nullptr;
            
            switch (expr->binary_op) {
                case ast::BinaryOp::Add:
                    return builder_->CreateAdd(left, right, "addtmp");
                case ast::BinaryOp::Sub:
                    return builder_->CreateSub(left, right, "subtmp");
                case ast::BinaryOp::Mul:
                    return builder_->CreateMul(left, right, "multmp");
                case ast::BinaryOp::Div:
                    return builder_->CreateSDiv(left, right, "divtmp");
                case ast::BinaryOp::Mod:
                    return builder_->CreateSRem(left, right, "modtmp");
                case ast::BinaryOp::Eq:
                    return builder_->CreateICmpEQ(left, right, "eqtmp");
                case ast::BinaryOp::Ne:
                    return builder_->CreateICmpNE(left, right, "netmp");
                case ast::BinaryOp::Lt:
                    return builder_->CreateICmpSLT(left, right, "lttmp");
                case ast::BinaryOp::Le:
                    return builder_->CreateICmpSLE(left, right, "letmp");
                case ast::BinaryOp::Gt:
                    return builder_->CreateICmpSGT(left, right, "gttmp");
                case ast::BinaryOp::Ge:
                    return builder_->CreateICmpSGE(left, right, "getmp");
                case ast::BinaryOp::And:
                    return builder_->CreateAnd(left, right, "andtmp");
                case ast::BinaryOp::Or:
                    return builder_->CreateOr(left, right, "ortmp");
                case ast::BinaryOp::BitAnd:
                    return builder_->CreateAnd(left, right, "bitandtmp");
                case ast::BinaryOp::BitOr:
                    return builder_->CreateOr(left, right, "bitortmp");
                case ast::BinaryOp::BitXor:
                    return builder_->CreateXor(left, right, "bitxortmp");
                case ast::BinaryOp::Shl:
                    return builder_->CreateShl(left, right, "shltmp");
                case ast::BinaryOp::Shr:
                    return builder_->CreateAShr(left, right, "shrtmp");
                default:
                    return nullptr;
            }
        }
            
        case ast::ExprKind::Call: {
            llvm::Value* callee = codegen_expr(expr->callee.get());
            if (!callee) return nullptr;
            
            std::vector<llvm::Value*> args;
            for (auto& arg : expr->arguments) {
                llvm::Value* arg_val = codegen_expr(arg.get());
                if (!arg_val) return nullptr;
                args.push_back(arg_val);
            }
            
            return builder_->CreateCall(llvm::cast<llvm::Function>(callee), args, "calltmp");
        }
            
        case ast::ExprKind::Block: {
            llvm::Value* result = nullptr;
            for (auto& stmt : expr->block_stmts) {
                codegen_stmt(stmt.get());
            }
            if (expr->block_expr) {
                result = codegen_expr(expr->block_expr.get());
            }
            return result;
        }
            
        case ast::ExprKind::If: {
            llvm::Value* cond_val = codegen_expr(expr->condition.get());
            if (!cond_val) return nullptr;
            
            llvm::Function* func = builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(*context_, "then", func);
            llvm::BasicBlock* else_bb = llvm::BasicBlock::Create(*context_, "else");
            llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context_, "ifcont");
            
            builder_->CreateCondBr(cond_val, then_bb, else_bb);
            
            // Then branch
            builder_->SetInsertPoint(then_bb);
            llvm::Value* then_val = codegen_expr(expr->then_branch.get());
            bool then_returns = builder_->GetInsertBlock()->getTerminator() != nullptr;
            if (!then_returns) {
                builder_->CreateBr(merge_bb);
            }
            then_bb = builder_->GetInsertBlock();
            
            // Else branch
            func->insert(func->end(), else_bb);
            builder_->SetInsertPoint(else_bb);
            llvm::Value* else_val = nullptr;
            if (expr->else_branch) {
                else_val = codegen_expr(expr->else_branch.get());
            }
            bool else_returns = builder_->GetInsertBlock()->getTerminator() != nullptr;
            if (!else_returns) {
                builder_->CreateBr(merge_bb);
            }
            else_bb = builder_->GetInsertBlock();
            
            // Merge - only if at least one branch doesn't return
            if (!then_returns || !else_returns) {
                func->insert(func->end(), merge_bb);
                builder_->SetInsertPoint(merge_bb);
                
                // Create PHI node if both branches produce values and don't return
                if (!then_returns && !else_returns && then_val && else_val && 
                        then_val->getType() == else_val->getType()) {
                    llvm::PHINode* phi = builder_->CreatePHI(then_val->getType(), 2, "iftmp");
                    phi->addIncoming(then_val, then_bb);
                    phi->addIncoming(else_val, else_bb);
                    return phi;
                }
            } else {
                // Both branches return - merge block is unreachable, don't create it
                delete merge_bb;
                // Builder is left pointing to the else block (which is terminated)
                // This is fine - subsequent code is unreachable
            }
            
            return nullptr;
        }
            
        case ast::ExprKind::Return:
            if (expr->return_value) {
                llvm::Value* ret_val = codegen_expr(expr->return_value.get());
                if (ret_val) {
                    builder_->CreateRet(ret_val);
                    return ret_val;
                }
            } else {
                builder_->CreateRetVoid();
            }
            return nullptr;
            
        default:
            // TODO: Handle other expression kinds
            return nullptr;
    }
}

llvm::Value* LLVMCodeGen::codegen_stmt(ast::Stmt* stmt) {
    if (!stmt) return nullptr;
    
    switch (stmt->kind) {
        case ast::StmtKind::Expr:
            return codegen_expr(stmt->expr.get());
        case ast::StmtKind::Let: {
            // Get variable name from pattern
            if (stmt->let_pattern && 
                stmt->let_pattern->kind == ast::PatternKind::Identifier &&
                stmt->let_pattern->binding_name) {
                const std::string& var_name = *stmt->let_pattern->binding_name;
                
                // TODO: Create alloca for mutable variables
                // For now, store values directly (SSA style)
                if (stmt->let_initializer) {
                    llvm::Value* init_val = codegen_expr(stmt->let_initializer.get());
                    if (init_val) {
                        named_values_[var_name] = init_val;
                    }
                }
            }
            return nullptr;
        }
        default:
            return nullptr;
    }
}

} // namespace apex::codegen
