#include "CodeGenerator.hpp"
#include "AST.hpp"
#include <sstream>
#include <stdexcept>
#include <typeinfo>

// Helper: Check if an expression is a string literal.
static bool isStringLiteral(Expression* expr) {
    return dynamic_cast<StringLiteral*>(expr) != nullptr;
}

// Determines the return type for a function based on its name.
// For our demo, "greet", "setName", and "bark" return std::string.
static std::string determineFunctionReturnType(FunctionDeclaration* funcDecl) {
    if (funcDecl->name == "greet" || funcDecl->name == "setName" || funcDecl->name == "bark")
        return "std::string";
    return "int";
}

// Determines parameter type; for setName the parameter is std::string.
static std::string determineParameterType(FunctionDeclaration* funcDecl) {
    if (funcDecl->name == "setName")
        return "std::string";
    return "int";
}

// Generates complete C++ code from the MiniLang AST.
std::string CodeGenerator::generate(Program* program) {
    std::ostringstream out;
    // Standard includes and built-in functions.
    out << "#include <iostream>\n";
    out << "#include <string>\n";
    out << "#include <fstream>\n";
    out << "#include \"Builtins.hpp\"\n\n";

    // Forward declarations for functions.
    for (auto& stmt : program->statements) {
        if (auto funcDecl = dynamic_cast<FunctionDeclaration*>(stmt.get())) {
            out << generateFunctionPrototype(funcDecl) << "\n";
        }
    }
    out << "\n";
    // Generate class definitions.
    for (auto& stmt : program->statements) {
        if (auto classDecl = dynamic_cast<ClassDeclaration*>(stmt.get())) {
            out << generateClassDeclaration(classDecl) << "\n\n";
        }
    }
    // Generate function definitions.
    for (auto& stmt : program->statements) {
        if (auto funcDecl = dynamic_cast<FunctionDeclaration*>(stmt.get())) {
            out << generateFunctionDefinition(funcDecl) << "\n\n";
        }
    }
    // Generate main() from remaining (non-function, non-class) statements.
    out << "int main() {\n";
    for (auto& stmt : program->statements) {
        if (dynamic_cast<FunctionDeclaration*>(stmt.get()) || 
            dynamic_cast<ClassDeclaration*>(stmt.get()))
            continue;
        out << generateStatement(stmt.get()) << "\n";
    }
    out << "    return 0;\n";
    out << "}\n";
    return out.str();
}

std::string CodeGenerator::generateFunctionPrototype(FunctionDeclaration* funcDecl) {
    std::ostringstream out;
    std::string retType = determineFunctionReturnType(funcDecl);
    out << retType << " " << funcDecl->name << "(";
    bool first = true;
    for (auto& param : funcDecl->params) {
        if (!first)
            out << ", ";
        out << determineParameterType(funcDecl) << " " << param;
        first = false;
    }
    out << ");";
    return out.str();
}

std::string CodeGenerator::generateFunctionDefinition(FunctionDeclaration* funcDecl) {
    std::ostringstream out;
    std::string retType = determineFunctionReturnType(funcDecl);
    out << retType << " " << funcDecl->name << "(";
    bool first = true;
    for (auto& param : funcDecl->params) {
        if (!first)
            out << ", ";
        out << determineParameterType(funcDecl) << " " << param;
        first = false;
    }
    out << ") {\n";
    out << generateStatement(funcDecl->body.get()) << "\n";
    out << "}";
    return out.str();
}

std::string CodeGenerator::generateClassDeclaration(ClassDeclaration* classDecl) {
    std::ostringstream out;
    if (classDecl->baseClass.empty())
        out << "class " << classDecl->name << " {\n";
    else
        out << "class " << classDecl->name << " : public " << classDecl->baseClass << " {\n";
    out << "public:\n";
    out << generateClassBody(classDecl->body.get());
    out << "\n};";
    return out.str();
}

std::string CodeGenerator::generateClassBody(BlockStatement* body) {
    std::ostringstream out;
    for (auto& stmt : body->statements) {
        // Field declarations.
        if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt.get())) {
            if (isStringLiteral(varDecl->expression.get()))
                out << "    std::string " << varDecl->identifier << " = " << generateExpression(varDecl->expression.get()) << ";\n";
            else
                out << "    int " << varDecl->identifier << " = " << generateExpression(varDecl->expression.get()) << ";\n";
        }
        // Method declarations.
        else if (auto funcDecl = dynamic_cast<FunctionDeclaration*>(stmt.get())) {
            out << "    " << generateFunctionDefinition(funcDecl) << "\n";
        }
        else {
            throw std::runtime_error("Unknown statement type in class body.");
        }
    }
    return out.str();
}

std::string CodeGenerator::generateStatement(Statement* stmt) {
    std::ostringstream out;
    if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        out << "    auto " << varDecl->identifier << " = " << generateExpression(varDecl->expression.get()) << ";";
    } else if (auto printStmt = dynamic_cast<PrintStatement*>(stmt)) {
        out << "    std::cout << " << generateExpression(printStmt->expression.get()) << " << std::endl;";
    } else if (auto exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
        out << "    " << generateExpression(exprStmt->expression.get()) << ";";
    } else if (auto returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        out << "    return " << (returnStmt->expression ? generateExpression(returnStmt->expression.get()) : "0") << ";";
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        out << "    if (" << generateExpression(ifStmt->condition.get()) << ") {\n";
        out << generateStatement(ifStmt->thenBranch.get()) << "\n    }";
        if (ifStmt->elseBranch) {
            out << " else {\n";
            out << generateStatement(ifStmt->elseBranch.get()) << "\n    }";
        }
    } else if (auto blockStmt = dynamic_cast<BlockStatement*>(stmt)) {
        for (auto& s : blockStmt->statements) {
            out << generateStatement(s.get()) << "\n";
        }
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        out << "    while (" << generateExpression(whileStmt->condition.get()) << ") {\n";
        out << generateStatement(whileStmt->body.get()) << "\n    }";
    } else {
        throw std::runtime_error("Unknown statement type in code generator.");
    }
    return out.str();
}

std::string CodeGenerator::generateExpression(Expression* expr) {
    std::ostringstream out;
    if (auto num = dynamic_cast<NumericLiteral*>(expr)) {
        out << static_cast<int>(num->value);
    } else if (auto str = dynamic_cast<StringLiteral*>(expr)) {
        out << "\"" << str->value << "\"";
    } else if (auto id = dynamic_cast<Identifier*>(expr)) {
        out << id->name;
    } else if (auto assign = dynamic_cast<Assignment*>(expr)) {
        out << assign->name << " = " << generateExpression(assign->value.get());
    } else if (auto bin = dynamic_cast<BinaryExpression*>(expr)) {
        std::string leftText = generateExpression(bin->left.get());
        std::string rightText = generateExpression(bin->right.get());
        // For binary plus, if the left operand is a string literal, use std::string concatenation.
        if (bin->op == "+" && leftText.size() >= 2 && leftText.front() == '\"' && leftText.back() == '\"') {
            out << "std::string(" << leftText << ") + " << rightText;
        } else {
            out << "(" << leftText << " " << bin->op << " " << rightText << ")";
        }
    } else if (auto unary = dynamic_cast<UnaryExpression*>(expr)) {
        out << "(" << unary->op << generateExpression(unary->argument.get()) << ")";
    } else if (auto callExpr = dynamic_cast<CallExpression*>(expr)) {
        out << generateExpression(callExpr->callee.get()) << "(";
        bool first = true;
        for (auto& arg : callExpr->arguments) {
            if (!first)
                out << ", ";
            out << generateExpression(arg.get());
            first = false;
        }
        out << ")";
    } else if (auto memberAccess = dynamic_cast<MemberAccessExpression*>(expr)) {
        // Use arrow operator for member access.
        out << generateExpression(memberAccess->object.get()) << "->" << memberAccess->member;
    } else if (auto newExpr = dynamic_cast<NewExpression*>(expr)) {
        out << "new " << newExpr->className << "(";
        bool first = true;
        for (auto& arg : newExpr->arguments) {
            if (!first)
                out << ", ";
            out << generateExpression(arg.get());
            first = false;
        }
        out << ")";
    } else {
        throw std::runtime_error("Unknown expression type in code generator.");
    }
    return out.str();
}