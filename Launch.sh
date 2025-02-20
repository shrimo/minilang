#!/bin/bash
# Launch.sh: Compile the generated program and run it.

# Check if compiled.cpp exists before compiling.
if [ ! -f compiled.cpp ]; then
    echo "Error: compiled.cpp not found. Please run the mini_compiler first."
    exit 1
fi

# Compile the generated C++ source along with Builtins.cpp into program.
g++ -std=c++17 compiled.cpp Builtins.cpp -o program

if [ $? -eq 0 ]; then
    echo "Program compiled successfully. Running program..."
    ./program
else
    echo "Error compiling program."
    exit 1
fi