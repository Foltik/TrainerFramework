#pragma once

#include "Lexer.h"
#include "AST.h"

enum class Operator;

class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer) {
        currToken = lexer.nextToken();
    }

    Program program();

private:
    using Token = Lexer::Token;
    using Type = Lexer::Token::Type;

    Operator opFromToken(const Token& token);

    void expect(Type type);
    void pop(Type type);
    void pop();

    Block block();

    Injection injection();

    Statement statement();
    Statement label();
    Statement directive();
    Statement instruction();

    Expression expression();
    Expression term();
    Expression factor();
    Expression variable();
    Expression reg();

    std::string_view string();

    template<typename T>
    Number number(Type type) {
        expect(type);
        auto num = std::get<T>(currToken.value);
        pop();
        return Number{num};
    }

    Token currToken;

    int lastLine = 0;
    int lastCol = 0;

    Lexer& lexer;
};

