#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <cctype>

enum class TokenType {
    NUMBER,
    STRING,
    IDENTIFIER,
    LET,
    PRINT,
    FUNCTION,
    IF,
    ELSE,
    WHILE,
    RETURN,
    CLASS,    // 'class' keyword.
    EXTENDS,  // 'extends' keyword.
    NEW,      // 'new' operator.
    THIS,     // 'this' keyword.
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    EQUALS,
    DOT,      // '.'
    SEMICOLON,
    LPAREN,   // '('
    RPAREN,   // ')'
    LBRACE,   // '{'
    RBRACE,   // '}'
    COMMA,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    double numberValue = 0;
};

class Lexer {
public:
    Lexer(const std::string &input);
    std::vector<Token> tokenize();
private:
    char peek() const;
    char get();
    void skipWhitespace();
    Token number();
    Token identifier();
    Token string();
    std::string input;
    size_t pos;
};

#endif // LEXER_HPP