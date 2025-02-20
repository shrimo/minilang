#include "Parser.hpp"
#include <stdexcept>
#include <memory>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

Token Parser::currentToken() {
    if (pos < tokens.size())
        return tokens[pos];
    Token t;
    t.type = TokenType::END_OF_FILE;
    return t;
}

void Parser::advance() {
    if (pos < tokens.size())
        pos++;
}

bool Parser::match(TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::SEMICOLON) {
            advance();
            continue;
        }
        program->statements.push_back(declaration());
    }
    return program;
}

std::unique_ptr<Statement> Parser::declaration() {
    if (currentToken().type == TokenType::CLASS)
        return classDeclaration();
    if (currentToken().type == TokenType::FUNCTION)
        return functionDeclaration();
    if (currentToken().type == TokenType::LET)
        return expressionStatement();
    return statement();
}

std::unique_ptr<Statement> Parser::classDeclaration() {
    advance(); // consume 'class'
    if (currentToken().type != TokenType::IDENTIFIER)
        throw std::runtime_error("Expected class name after 'class'");
    auto classDecl = std::make_unique<ClassDeclaration>();
    classDecl->name = currentToken().lexeme;
    advance();
    // Optional "extends" clause.
    if (match(TokenType::EXTENDS)) {
        if (currentToken().type != TokenType::IDENTIFIER)
            throw std::runtime_error("Expected base class name after 'extends'");
        classDecl->baseClass = currentToken().lexeme;
        advance();
    }
    classDecl->body = block();
    return classDecl;
}

std::unique_ptr<Statement> Parser::functionDeclaration() {
    advance(); // consume "function"
    if (currentToken().type != TokenType::IDENTIFIER)
        throw std::runtime_error("Expected function name after 'function'");
    std::string fname = currentToken().lexeme;
    advance();
    if (!match(TokenType::LPAREN))
        throw std::runtime_error("Expected '(' after function name");
    std::vector<std::string> params;
    if (currentToken().type != TokenType::RPAREN) {
        do {
            if (currentToken().type != TokenType::IDENTIFIER)
                throw std::runtime_error("Expected parameter name");
            params.push_back(currentToken().lexeme);
            advance();
        } while (match(TokenType::COMMA));
    }
    if (!match(TokenType::RPAREN))
        throw std::runtime_error("Expected ')' after parameters");
    auto body = block();
    auto funcDecl = std::make_unique<FunctionDeclaration>();
    funcDecl->name = fname;
    funcDecl->params = params;
    funcDecl->body = std::move(body);
    return funcDecl;
}

std::unique_ptr<BlockStatement> Parser::block() {
    if (!match(TokenType::LBRACE))
        throw std::runtime_error("Expected '{' to start block");
    auto blockStmt = std::make_unique<BlockStatement>();
    while (currentToken().type != TokenType::RBRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::SEMICOLON) {
            advance();
            continue;
        }
        blockStmt->statements.push_back(declaration());
    }
    if (!match(TokenType::RBRACE))
        throw std::runtime_error("Expected '}' after block");
    return blockStmt;
}

std::unique_ptr<Statement> Parser::ifStatement() {
    advance(); // consume 'if'
    if (!match(TokenType::LPAREN))
        throw std::runtime_error("Expected '(' after 'if'");
    auto condition = expression();
    if (!match(TokenType::RPAREN))
        throw std::runtime_error("Expected ')' after if condition");
    auto thenBranch = block();
    std::unique_ptr<BlockStatement> elseBranch = nullptr;
    if (currentToken().type == TokenType::ELSE) {
        advance();
        elseBranch = block();
    }
    auto ifStmt = std::make_unique<IfStatement>();
    ifStmt->condition = std::move(condition);
    ifStmt->thenBranch = std::move(thenBranch);
    ifStmt->elseBranch = std::move(elseBranch);
    return ifStmt;
}

std::unique_ptr<Statement> Parser::whileStatement() {
    advance(); // consume 'while'
    if (!match(TokenType::LPAREN))
        throw std::runtime_error("Expected '(' after 'while'");
    auto condition = expression();
    if (!match(TokenType::RPAREN))
        throw std::runtime_error("Expected ')' after while condition");
    auto body = block();
    auto whileStmt = std::make_unique<WhileStatement>();
    whileStmt->condition = std::move(condition);
    whileStmt->body = std::move(body);
    return whileStmt;
}

std::unique_ptr<Statement> Parser::returnStatement() {
    advance(); // consume 'return'
    auto retStmt = std::make_unique<ReturnStatement>();
    if (currentToken().type != TokenType::SEMICOLON)
        retStmt->expression = expression();
    if (!match(TokenType::SEMICOLON))
        throw std::runtime_error("Expected ';' after return statement");
    return retStmt;
}

std::unique_ptr<Statement> Parser::expressionStatement() {
    if (currentToken().type == TokenType::LET) {
        advance();
        if (currentToken().type != TokenType::IDENTIFIER)
            throw std::runtime_error("Expected identifier in variable declaration.");
        std::string varName = currentToken().lexeme;
        advance();
        if (!match(TokenType::EQUALS))
            throw std::runtime_error("Expected '=' in variable declaration.");
        auto expr = expression();
        if (!match(TokenType::SEMICOLON))
            throw std::runtime_error("Expected ';' after variable declaration.");
        auto varDecl = std::make_unique<VariableDeclaration>();
        varDecl->identifier = varName;
        varDecl->expression = std::move(expr);
        return varDecl;
    }
    if (currentToken().type == TokenType::PRINT) {
        advance();
        auto expr = expression();
        if (!match(TokenType::SEMICOLON))
            throw std::runtime_error("Expected ';' after print statement.");
        auto printStmt = std::make_unique<PrintStatement>();
        printStmt->expression = std::move(expr);
        return printStmt;
    }
    if (currentToken().type == TokenType::RETURN)
        return returnStatement();
    if (currentToken().type == TokenType::IF)
        return ifStatement();
    if (currentToken().type == TokenType::WHILE)
        return whileStatement();
    auto expr = expression();
    if (!match(TokenType::SEMICOLON))
        throw std::runtime_error("Expected ';' after expression.");
    auto exprStmt = std::make_unique<ExpressionStatement>();
    exprStmt->expression = std::move(expr);
    return exprStmt;
}

std::unique_ptr<Statement> Parser::statement() {
    return expressionStatement();
}

std::unique_ptr<Expression> Parser::assignment() {
    auto expr = equality();
    if (currentToken().type == TokenType::EQUALS) {
        advance(); // consume '='
        auto value = assignment();
        if (auto ident = dynamic_cast<Identifier*>(expr.get())) {
            auto assign = std::make_unique<Assignment>();
            assign->name = ident->name;
            assign->value = std::move(value);
            return assign;
        }
        throw std::runtime_error("Invalid assignment target.");
    }
    return expr;
}

std::unique_ptr<Expression> Parser::equality() {
    return comparison();
}

std::unique_ptr<Expression> Parser::comparison() {
    auto expr = addition();
    while (currentToken().type == TokenType::LESS ||
           currentToken().type == TokenType::LESS_EQUAL ||
           currentToken().type == TokenType::GREATER ||
           currentToken().type == TokenType::GREATER_EQUAL) {
        std::string op = currentToken().lexeme;
        advance();
        auto right = addition();
        auto binExp = std::make_unique<BinaryExpression>();
        binExp->op = op;
        binExp->left = std::move(expr);
        binExp->right = std::move(right);
        expr = std::move(binExp);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::addition() {
    auto expr = multiplication();
    while (currentToken().type == TokenType::PLUS ||
           currentToken().type == TokenType::MINUS) {
        std::string op = currentToken().lexeme;
        advance();
        auto right = multiplication();
        auto binExp = std::make_unique<BinaryExpression>();
        binExp->op = op;
        binExp->left = std::move(expr);
        binExp->right = std::move(right);
        expr = std::move(binExp);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::multiplication() {
    auto expr = call();
    while (currentToken().type == TokenType::MULTIPLY ||
           currentToken().type == TokenType::DIVIDE) {
        std::string op = currentToken().lexeme;
        advance();
        auto right = call();
        auto binExp = std::make_unique<BinaryExpression>();
        binExp->op = op;
        binExp->left = std::move(expr);
        binExp->right = std::move(right);
        expr = std::move(binExp);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::call() {
    auto expr = primary();
    while (true) {
        if (match(TokenType::LPAREN)) {
            std::vector<std::unique_ptr<Expression>> args;
            if (currentToken().type != TokenType::RPAREN) {
                do {
                    args.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            if (!match(TokenType::RPAREN))
                throw std::runtime_error("Expected ')' after arguments");
            auto callExpr = std::make_unique<CallExpression>();
            callExpr->callee = std::move(expr);
            callExpr->arguments = std::move(args);
            expr = std::move(callExpr);
        } else if (match(TokenType::DOT)) {
            if (currentToken().type != TokenType::IDENTIFIER)
                throw std::runtime_error("Expected property name after '.'");
            std::string memberName = currentToken().lexeme;
            advance();
            auto memberAccess = std::make_unique<MemberAccessExpression>();
            memberAccess->object = std::move(expr);
            memberAccess->member = memberName;
            expr = std::move(memberAccess);
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<Expression> Parser::primary() {
    Token token = currentToken();
    if (token.type == TokenType::NUMBER) {
        advance();
        auto numLit = std::make_unique<NumericLiteral>();
        numLit->value = token.numberValue;
        return numLit;
    } else if (token.type == TokenType::STRING) {
        advance();
        auto strLit = std::make_unique<StringLiteral>();
        strLit->value = token.lexeme;
        return strLit;
    } else if (token.type == TokenType::IDENTIFIER) {
        advance();
        auto id = std::make_unique<Identifier>();
        id->name = token.lexeme;
        return id;
    } else if (token.type == TokenType::NEW) {
        advance(); // consume 'new'
        if (currentToken().type != TokenType::IDENTIFIER)
            throw std::runtime_error("Expected class name after 'new'");
        auto newExpr = std::make_unique<NewExpression>();
        newExpr->className = currentToken().lexeme;
        advance();
        if (!match(TokenType::LPAREN))
            throw std::runtime_error("Expected '(' after class name in new expression");
        while (currentToken().type != TokenType::RPAREN) {
            newExpr->arguments.push_back(expression());
            if (!match(TokenType::COMMA))
                break;
        }
        if (!match(TokenType::RPAREN))
            throw std::runtime_error("Expected ')' after arguments in new expression");
        return newExpr;
    } else if (token.type == TokenType::LPAREN) {
        advance();
        auto expr = expression();
        if (!match(TokenType::RPAREN))
            throw std::runtime_error("Expected ')'");
        return expr;
    } else if (token.type == TokenType::MINUS) {
        advance();
        auto arg = primary();
        auto unary = std::make_unique<UnaryExpression>();
        unary->op = "-";
        unary->argument = std::move(arg);
        return unary;
    }
    throw std::runtime_error("Unexpected token in expression: " + token.lexeme);
}

std::unique_ptr<Expression> Parser::expression() {
    return assignment();
}