cmake_minimum_required(VERSION 3.20)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Compiler sources
set(APEXC_SOURCES
    main.cpp
    lexer/Token.cpp
    lexer/Lexer.cpp
    parser/Parser.cpp
    sema/SemanticAnalyzer.cpp
    codegen/LLVMCodeGen.cpp
)

# Create executable
add_executable(apexc ${APEXC_SOURCES})

# Link LLVM libraries - include all necessary components for native target
llvm_map_components_to_libnames(llvm_libs
    core
    support
    irreader
    target
    x86codegen
    x86asmparser
    x86desc
    x86info
    aarch64codegen
    aarch64asmparser
    aarch64desc
    aarch64info
    nativecodegen
)

target_link_libraries(apexc ${llvm_libs})

# Set C++20 standard
set_target_properties(apexc PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Installation
install(TARGETS apexc DESTINATION bin)
