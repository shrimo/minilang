#include "Lexer.hpp"
#include <stdexcept>
#include <cstdlib>

Lexer::Lexer(const std::string &input) : input(input), pos(0) {}

char Lexer::peek() const {
    if (pos < input.size())
        return input[pos];
    return '\0';
}

char Lexer::get() {
    if (pos < input.size())
        return input[pos++];
    return '\0';
}

void Lexer::skipWhitespace() {
    while (pos < input.size()) {
        char current = input[pos];
        if (std::isspace(current)) {
            pos++;
        } else if (current == '/' && pos + 1 < input.size() && input[pos + 1] == '/') {
            pos += 2;
            while (pos < input.size() && input[pos] != '\n')
                pos++;
        } else {
            break;
        }
    }
}

Token Lexer::number() {
    size_t start = pos;
    while (pos < input.size() && std::isdigit(input[pos]))
        pos++;
    if (pos < input.size() && input[pos] == '.') {
        pos++;
        while (pos < input.size() && std::isdigit(input[pos]))
            pos++;
    }
    std::string numStr = input.substr(start, pos - start);
    Token token;
    token.type = TokenType::NUMBER;
    token.lexeme = numStr;
    token.numberValue = std::stod(numStr);
    return token;
}

Token Lexer::string() {
    char quote = get(); // consume opening quote.
    size_t start = pos;
    while (pos < input.size() && input[pos] != quote)
        pos++;
    if (pos >= input.size())
        throw std::runtime_error("Unterminated string literal.");
    std::string strVal = input.substr(start, pos - start);
    get(); // consume closing quote.
    Token token;
    token.type = TokenType::STRING;
    token.lexeme = strVal;
    return token;
}

Token Lexer::identifier() {
    size_t start = pos;
    while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '_'))
        pos++;
    std::string idStr = input.substr(start, pos - start);
    Token token;
    if (idStr == "let")
        token.type = TokenType::LET;
    else if (idStr == "print")
        token.type = TokenType::PRINT;
    else if (idStr == "function")
        token.type = TokenType::FUNCTION;
    else if (idStr == "if")
        token.type = TokenType::IF;
    else if (idStr == "else")
        token.type = TokenType::ELSE;
    else if (idStr == "while")
        token.type = TokenType::WHILE;
    else if (idStr == "return")
        token.type = TokenType::RETURN;
    else if (idStr == "class")
        token.type = TokenType::CLASS;
    else if (idStr == "extends")
        token.type = TokenType::EXTENDS;
    else if (idStr == "new")
        token.type = TokenType::NEW;
    else if (idStr == "this")
        token.type = TokenType::THIS;
    else
        token.type = TokenType::IDENTIFIER;
    token.lexeme = idStr;
    return token;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < input.size()) {
        skipWhitespace();
        char current = peek();
        if (current == '\0')
            break;
        if (std::isdigit(current)) {
            tokens.push_back(number());
            continue;
        }
        if (current == '"') {
            tokens.push_back(string());
            continue;
        }
        if (std::isalpha(current) || current == '_') {
            tokens.push_back(identifier());
            continue;
        }
        Token token;
        switch (current) {
            case '<':
                pos++;
                if (peek() == '=') {
                    pos++;
                    token.type = TokenType::LESS_EQUAL;
                    token.lexeme = "<=";
                } else {
                    token.type = TokenType::LESS;
                    token.lexeme = "<";
                }
                tokens.push_back(token);
                continue;
            case '>':
                pos++;
                if (peek() == '=') {
                    pos++;
                    token.type = TokenType::GREATER_EQUAL;
                    token.lexeme = ">=";
                } else {
                    token.type = TokenType::GREATER;
                    token.lexeme = ">";
                }
                tokens.push_back(token);
                continue;
            case '+': token.type = TokenType::PLUS; token.lexeme = "+"; break;
            case '-': token.type = TokenType::MINUS; token.lexeme = "-"; break;
            case '*': token.type = TokenType::MULTIPLY; token.lexeme = "*"; break;
            case '/': token.type = TokenType::DIVIDE; token.lexeme = "/"; break;
            case '=': token.type = TokenType::EQUALS; token.lexeme = "="; break;
            case '.': token.type = TokenType::DOT; token.lexeme = "."; break;
            case ';': token.type = TokenType::SEMICOLON; token.lexeme = ";"; break;
            case '(': token.type = TokenType::LPAREN; token.lexeme = "("; break;
            case ')': token.type = TokenType::RPAREN; token.lexeme = ")"; break;
            case '{': token.type = TokenType::LBRACE; token.lexeme = "{"; break;
            case '}': token.type = TokenType::RBRACE; token.lexeme = "}"; break;
            case ',': token.type = TokenType::COMMA; token.lexeme = ","; break;
            default:
                token.type = TokenType::UNKNOWN;
                token.lexeme = std::string(1, current);
                break;
        }
        tokens.push_back(token);
        pos++;
    }
    Token eofToken;
    eofToken.type = TokenType::END_OF_FILE;
    eofToken.lexeme = "";
    tokens.push_back(eofToken);
    return tokens;
}