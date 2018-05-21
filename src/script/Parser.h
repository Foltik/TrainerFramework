#pragma once

#include "Lexer.h"
#include "AST.h"

class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer)
                         , currToken(lexer.nextToken()) {}

    Program program();

private:
    using Token = Lexer::Token;
    using Type = Lexer::Token::Type;

    void expectNext(const Type& type);

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

    Token currToken;
    Lexer& lexer;
};

