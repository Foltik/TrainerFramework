#include "Parser.h"

#include "AST.h"

#include <iostream> //TODO: REMOVE

void Parser::expectNext(const Type& type) {
    if (currToken.type == type)
        currToken = lexer.nextToken();
    else
        std::cout << "error yo" << std::endl;
}

Program Parser::program() {
    Program prog;

    while (currToken.type != Type::Eof)
        prog.blocks.push_back(block());

    return prog;
}

Block Parser::block() {
    Block block;

    expectNext(Type::Eol);
    block.injection = injection();
    expectNext(Type::Eol);

    while (currToken.type != Type::Eol && currToken.type != Type::Eof)
        block.statements.push_back(statement());

    return block;
}

Injection Parser::injection() {
    Injection inj;

    expectNext(Type::Lcurl);
    inj.expr = expression();
    expectNext(Type::Rcurl);
    expectNext(Type::Colon);

    return inj;
}




namespace {
    using Type = Lexer::Token::Type;
    using Op = Operator;

    Op opFromToken(Type type) {
        switch(type) {
            case Type::Plus:
                return Op::Plus;
            case Type::Minus:
                return Op::Minus;
            case Type::Mul:
                return Op::Mul;
            case Type::Div:
                return Op::Div;
        }
    }
}

Expression Parser::expression() {
    Expression expr;

    expr = term();

    while (currToken.type == Type::Plus || currToken.type == Type::Minus) {
        Token token = currToken;
        expectNext(token.type);

        BinaryOp op{expr, opFromToken(token.type), term()};

        expr = {op};
    }

    return expr;
}

Expression Parser::term() {
    Expression expr;

    expr = factor();

    while (currToken.type == Type::Mul || currToken.type == Type::Div) {
        Token token = currToken;
        expectNext(token.type);

        BinaryOp op{expr, opFromToken(token.type), factor()};

        expr = {op};
    }

    return expr;
}

Expression Parser::factor() {
    Token token = currToken;
    if (token.type == Type::Plus) {
        expectNext(Type::Plus);
        UnaryOp op{opFromToken(token.type), factor()};
        return op;
    } else if (token.type == Type::Minus) {
        expectNext(Type::Minus);
        UnaryOp op{opFromToken(token.type), factor()};
        return op;
    } else if (token.type == Type::Integer) {
        Number num{std::get<long>(token.value)};
        expectNext(Type::Integer);
        return num;
    } else if (token.type == Type::Float) {
        Number num{std::get<float>(token.value)};
        expectNext(Type::Float);
        return num;
    } else if (token.type == Type::Lbracket) {
        expectNext(Type::Lbracket);
        Dereference deref{expression()};
        expectNext(Type::Rbracket);
        return deref;
    } else if (token.type == Type::Lparen) {
        expectNext(Type::Lparen);
        Expression expr = expression();
        expectNext(Type::Rparen);
        return expr;
    } else {
        return variable();
    }
}

Expression Parser::variable() {
    Identifier id;
    id.name = std::get<std::string_view>(currToken.value);
    expectNext(Type::Id);
    return {id};
}



Statement Parser::statement() {
    Token token = currToken;
    if (token.type == Type::Dot) {
        return directive();
    } else if (token.type == Type::Dollar){
        return label();
    } else {
        return instruction();
    }
}

Statement Parser::label() {
    expectNext(Type::Dollar);

    Label label;
    label.id.name = std::get<std::string_view>(currToken.value);
    expectNext(Type::Id);

    expectNext(Type::Colon);
    expectNext(Type::Eol);
    return {label};
}

Statement Parser::directive() {
    expectNext(Type::Dot);

    Directive directive;
    directive.id.name = std::get<std::string_view>(currToken.value);
    expectNext(Type::Id);

    while (currToken.type != Type::Eol) {
        directive.args.emplace_back(expression());
        if (currToken.type == Type::Comma)
            expectNext(Type::Comma);
    }

    expectNext(Type::Eol);
    return {directive};
}

Statement Parser::instruction() {
    Instruction instr;
    instr.id.name = std::get<std::string_view>(currToken.value);
    expectNext(Type::Id);

    while (currToken.type != Type::Eol)
        instr.args.emplace_back(expression());

    expectNext(Type::Eol);

    return {instr};
}

