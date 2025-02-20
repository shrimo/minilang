#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "AST.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

// The Value type supports numbers and strings.
struct Value {
    enum Type { NUMBER, STRING } type;
    double numberValue;
    std::string stringValue;

    Value() : type(NUMBER), numberValue(0) {}
    Value(double num) : type(NUMBER), numberValue(num) {}
    Value(const std::string &str) : type(STRING), stringValue(str) {}
};

class Interpreter {
public:
    Interpreter();
    void interpret(Program* program);

private:
    // Environment: mapping variable names to Value.
    std::vector<std::unordered_map<std::string, Value>> environments;
    std::unordered_map<std::string, FunctionDeclaration*> functions;

    Value visit(Expression* expr);
    void execute(Statement* stmt);
    void executeBlock(BlockStatement* block);

    Value lookupVariable(const std::string& name);
    void assignVariable(const std::string& name, const Value& value);
    void declareVariable(const std::string& name, const Value& value);

    Value callFunction(FunctionDeclaration* funcDecl, const std::vector<Value>& args);

    // For built-in functions.
    Value builtInFunction(const std::string& name, const std::vector<Value>& args);

    class ReturnException : public std::exception {
    public:
        Value value;
        ReturnException(const Value& value) : value(value) {}
    };
};

#endif // INTERPRETER_HPP