cmake_minimum_required(VERSION 3.20)
project(Apex VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    # Note: Not using -Werror to avoid issues with LLVM headers
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Options
option(APEX_BUILD_TESTS "Build tests" ON)
option(APEX_BUILD_TOOLS "Build tools (LSP, formatter, linter)" ON)
option(APEX_BUILD_EXAMPLES "Build example programs" ON)
option(APEX_USE_LLVM "Use LLVM backend" ON)

# Find LLVM
if(APEX_USE_LLVM)
    find_package(LLVM REQUIRED CONFIG)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    
    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})
    
    llvm_map_components_to_libnames(llvm_libs
        core
        support
        target
        irreader
        codegen
        mc
        mcparser
        option
        x86codegen
        x86asmparser
        aarch64codegen
        aarch64asmparser
    )
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Subdirectories
add_subdirectory(src/apexc)

if(APEX_BUILD_TOOLS)
    # add_subdirectory(src/tools)  # TODO: Not implemented yet
endif()

if(APEX_BUILD_TESTS)
    enable_testing()
    # add_subdirectory(tests)  # TODO: Not implemented yet
endif()

if(APEX_BUILD_EXAMPLES)
    # add_subdirectory(examples)  # TODO: Not implemented yet
endif()

# Install
install(TARGETS apexc
    RUNTIME DESTINATION bin
)

# Tools installation will be added when implemented
# if(APEX_BUILD_TOOLS)
#     install(TARGETS apexfmt apexlint
#         RUNTIME DESTINATION bin
#     )
# endif()
