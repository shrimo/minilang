#!/bin/bash
# Build.sh: Build the MiniLang compiler.

# Compile the compiler source files into the mini_compiler executable.
g++ -std=c++17 main.cpp Lexer.cpp Parser.cpp CodeGenerator.cpp Builtins.cpp -o mini_compiler

if [ $? -eq 0 ]; then
    echo "mini_compiler built successfully."
else
    echo "Error building mini_compiler."
    exit 1
fi