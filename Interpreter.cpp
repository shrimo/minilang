#include "Interpreter.hpp"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>

Interpreter::Interpreter() {
    environments.emplace_back(); // Global environment.
}

void Interpreter::interpret(Program* program) {
    // First pass: register function declarations.
    for (auto& stmt : program->statements) {
        if (auto funcDecl = dynamic_cast<FunctionDeclaration*>(stmt.get()))
            functions[funcDecl->name] = funcDecl;
    }
    // Execute non-function statements.
    for (auto& stmt : program->statements) {
        if (dynamic_cast<FunctionDeclaration*>(stmt.get()))
            continue;
        execute(stmt.get());
    }
}

void Interpreter::execute(Statement* stmt) {
    if (auto varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        Value value = visit(varDecl->expression.get());
        declareVariable(varDecl->identifier, value);
    } else if (auto printStmt = dynamic_cast<PrintStatement*>(stmt)) {
        Value value = visit(printStmt->expression.get());
        if (value.type == Value::NUMBER)
            std::cout << value.numberValue << std::endl;
        else
            std::cout << value.stringValue << std::endl;
    } else if (auto exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
        visit(exprStmt->expression.get());
    } else if (auto blockStmt = dynamic_cast<BlockStatement*>(stmt)) {
        executeBlock(blockStmt);
    } else if (auto ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        Value cond = visit(ifStmt->condition.get());
        bool condition = false;
        if (cond.type == Value::NUMBER)
            condition = (cond.numberValue != 0);
        else
            condition = !cond.stringValue.empty();
        if (condition)
            executeBlock(ifStmt->thenBranch.get());
        else if (ifStmt->elseBranch)
            executeBlock(ifStmt->elseBranch.get());
    } else if (auto whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        while (true) {
            Value cond = visit(whileStmt->condition.get());
            bool condition = false;
            if (cond.type == Value::NUMBER)
                condition = (cond.numberValue != 0);
            else
                condition = !cond.stringValue.empty();
            if (!condition)
                break;
            executeBlock(whileStmt->body.get());
        }
    } else if (auto returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        Value retVal;
        if (returnStmt->expression)
            retVal = visit(returnStmt->expression.get());
        throw ReturnException(retVal);
    } else if (dynamic_cast<FunctionDeclaration*>(stmt)) {
        // Already handled.
        return;
    } else {
        throw std::runtime_error("Unknown statement type in execute.");
    }
}

void Interpreter::executeBlock(BlockStatement* block) {
    environments.push_back({});
    try {
        for (auto& stmt : block->statements) {
            execute(stmt.get());
        }
    } catch (...) {
        environments.pop_back();
        throw;
    }
    environments.pop_back();
}

Value Interpreter::visit(Expression* expr) {
    if (auto num = dynamic_cast<NumericLiteral*>(expr))
        return Value(num->value);
    else if (auto str = dynamic_cast<StringLiteral*>(expr))
        return Value(str->value);
    else if (auto id = dynamic_cast<Identifier*>(expr))
        return lookupVariable(id->name);
    else if (auto assign = dynamic_cast<Assignment*>(expr)) {
        Value value = visit(assign->value.get());
        assignVariable(assign->name, value);
        return value;
    } else if (auto bin = dynamic_cast<BinaryExpression*>(expr)) {
        Value left = visit(bin->left.get());
        Value right = visit(bin->right.get());
        if (bin->op == "+") {
            if (left.type == Value::NUMBER && right.type == Value::NUMBER)
                return Value(left.numberValue + right.numberValue);
            else
                return Value((left.type == Value::STRING ? left.stringValue : std::to_string(left.numberValue)) +
                             (right.type == Value::STRING ? right.stringValue : std::to_string(right.numberValue)));
        } else if (bin->op == "-") {
            return Value(left.numberValue - right.numberValue);
        } else if (bin->op == "*") {
            return Value(left.numberValue * right.numberValue);
        } else if (bin->op == "/") {
            return Value(left.numberValue / right.numberValue);
        } else if (bin->op == "<") {
            return Value((left.numberValue < right.numberValue) ? 1.0 : 0.0);
        } else if (bin->op == "<=") {
            return Value((left.numberValue <= right.numberValue) ? 1.0 : 0.0);
        } else if (bin->op == ">") {
            return Value((left.numberValue > right.numberValue) ? 1.0 : 0.0);
        } else if (bin->op == ">=") {
            return Value((left.numberValue >= right.numberValue) ? 1.0 : 0.0);
        }
        throw std::runtime_error("Unknown binary operator: " + bin->op);
    } else if (auto unary = dynamic_cast<UnaryExpression*>(expr)) {
        Value arg = visit(unary->argument.get());
        if (unary->op == "-")
            return Value(-arg.numberValue);
        throw std::runtime_error("Unknown unary operator: " + unary->op);
    } else if (auto callExpr = dynamic_cast<CallExpression*>(expr)) {
        auto calleeId = dynamic_cast<Identifier*>(callExpr->callee.get());
        if (!calleeId)
            throw std::runtime_error("Can only call functions identified by name.");
        std::string funcName = calleeId->name;
        std::vector<Value> args;
        for (auto& arg : callExpr->arguments)
            args.push_back(visit(arg.get()));
        // Check for built-in functions first.
        if (funcName == "readFile" || funcName == "writeFile")
            return builtInFunction(funcName, args);
        if (functions.find(funcName) == functions.end())
            throw std::runtime_error("Undefined function: " + funcName);
        return callFunction(functions[funcName], args);
    }
    throw std::runtime_error("Unknown expression type in visit.");
}

Value Interpreter::callFunction(FunctionDeclaration* funcDecl, const std::vector<Value>& args) {
    environments.push_back({});
    for (size_t i = 0; i < funcDecl->params.size(); i++) {
        Value argVal = (i < args.size() ? args[i] : Value(0));
        environments.back()[funcDecl->params[i]] = argVal;
    }
    Value retVal;
    try {
        executeBlock(funcDecl->body.get());
    } catch (const ReturnException& e) {
        retVal = e.value;
    }
    environments.pop_back();
    return retVal;
}

Value Interpreter::lookupVariable(const std::string& name) {
    for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
        if (it->find(name) != it->end())
            return it->at(name);
    }
    throw std::runtime_error("Undefined variable: " + name);
}

void Interpreter::assignVariable(const std::string& name, const Value& value) {
    for (auto it = environments.rbegin(); it != environments.rend(); ++it) {
        if (it->find(name) != it->end()) {
            (*it)[name] = value;
            return;
        }
    }
    throw std::runtime_error("Undefined variable: " + name);
}

void Interpreter::declareVariable(const std::string& name, const Value& value) {
    environments.back()[name] = value;
}

Value Interpreter::builtInFunction(const std::string& name, const std::vector<Value>& args) {
    if (name == "readFile") {
        if (args.size() < 1 || args[0].type != Value::STRING)
            throw std::runtime_error("readFile expects a string filename.");
        std::ifstream file(args[0].stringValue);
        if (!file)
            throw std::runtime_error("Could not open file: " + args[0].stringValue);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return Value(buffer.str());
    } else if (name == "writeFile") {
        if (args.size() < 2 || args[0].type != Value::STRING || args[1].type != Value::STRING)
            throw std::runtime_error("writeFile expects two string arguments: filename and content.");
        std::ofstream file(args[0].stringValue);
        if (!file)
            throw std::runtime_error("Could not write to file: " + args[0].stringValue);
        file << args[1].stringValue;
        return Value(0);
    }
    throw std::runtime_error("Undefined built-in function: " + name);
}