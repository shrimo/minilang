#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

#include "AST.hpp"
#include <string>

class CodeGenerator {
public:
    // Generates complete C++ source code from a MiniLang program.
    std::string generate(Program* program);
private:
    std::string generateFunctionPrototype(FunctionDeclaration* funcDecl);
    std::string generateFunctionDefinition(FunctionDeclaration* funcDecl);
    std::string generateClassDeclaration(ClassDeclaration* classDecl);
    std::string generateClassBody(BlockStatement* body);
    std::string generateStatement(Statement* stmt);
    std::string generateExpression(Expression* expr);
};

#endif // CODEGENERATOR_HPP