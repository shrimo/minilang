#ifndef PARSER_HPP
#define PARSER_HPP

#include "AST.hpp"
#include "Lexer.hpp"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();
private:
    const std::vector<Token>& tokens;
    size_t pos;
    Token currentToken();
    void advance();
    bool match(TokenType type);

    // Declarations and statements.
    std::unique_ptr<Statement> declaration();
    std::unique_ptr<Statement> functionDeclaration();
    std::unique_ptr<Statement> classDeclaration();
    std::unique_ptr<BlockStatement> block();
    std::unique_ptr<Statement> ifStatement();
    std::unique_ptr<Statement> whileStatement();
    std::unique_ptr<Statement> returnStatement();
    std::unique_ptr<Statement> expressionStatement();
    std::unique_ptr<Statement> statement();

    // Expressions.
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> addition();
    std::unique_ptr<Expression> multiplication();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
};

#endif // PARSER_HPP