#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"
#include "AST.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: mini_compiler <source.minilang>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Cannot open file: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // Lexing.
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // Parsing.
    Parser parser(tokens);
    std::unique_ptr<Program> program = parser.parse();

    // Code Generation.
    CodeGenerator generator;
    std::string cppCode = generator.generate(program.get());

    std::ofstream out("compiled.cpp");
    if (!out) {
        std::cerr << "Error: Cannot write output file compiled.cpp" << std::endl;
        return 1;
    }
    out << cppCode;
    out.close();

    std::cout << "C++ source code generated to compiled.cpp" << std::endl;
    std::cout << "Now compile it with your C++ compiler (e.g., g++ -std=c++17 compiled.cpp Builtins.cpp -o program)" << std::endl;
    return 0;
}