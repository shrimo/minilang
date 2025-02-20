#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>

// Base AST node.
struct ASTNode {
    virtual ~ASTNode() = default;
};

// Expressions.
struct Expression : public ASTNode {};

// Numeric literal.
struct NumericLiteral : public Expression {
    double value;
};

// String literal.
struct StringLiteral : public Expression {
    std::string value;
};

// Identifier.
struct Identifier : public Expression {
    std::string name;
};

// Assignment.
struct Assignment : public Expression {
    std::string name;
    std::unique_ptr<Expression> value;
};

// Binary expression (e.g., +, -, *, /, etc.).
struct BinaryExpression : public Expression {
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

// Unary expression (e.g., -expr).
struct UnaryExpression : public Expression {
    std::string op;
    std::unique_ptr<Expression> argument;
};

// Call expression (for function/method calls).
struct CallExpression : public Expression {
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
};

// Member access: object.member.
struct MemberAccessExpression : public Expression {
    std::unique_ptr<Expression> object;
    std::string member;
};

// New expression: new ClassName(arg, ...).
struct NewExpression : public Expression {
    std::string className;
    std::vector<std::unique_ptr<Expression>> arguments;
};

// Statements.
struct Statement : public ASTNode {};

// Variable declaration.
struct VariableDeclaration : public Statement {
    std::string identifier;
    std::unique_ptr<Expression> expression;
};

// Print statement.
struct PrintStatement : public Statement {
    std::unique_ptr<Expression> expression;
};

// Expression statement.
struct ExpressionStatement : public Statement {
    std::unique_ptr<Expression> expression;
};

// Return statement.
struct ReturnStatement : public Statement {
    std::unique_ptr<Expression> expression; // May be nullptr.
};

// Block statement.
struct BlockStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
};

// If statement.
struct IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> thenBranch;
    std::unique_ptr<BlockStatement> elseBranch; // Optional.
};

// While statement.
struct WhileStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;
};

// Function declaration.
struct FunctionDeclaration : public Statement {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<BlockStatement> body;
};

// Class declaration.
// The class can optionally extend a base class.
struct ClassDeclaration : public Statement {
    std::string name;
    std::string baseClass; // Empty if no inheritance.
    // The class body is a block statement containing field and method declarations.
    std::unique_ptr<BlockStatement> body;
};

// Program (a list of statements).
struct Program : public ASTNode {
    std::vector<std::unique_ptr<Statement>> statements;
};

#endif // AST_HPP