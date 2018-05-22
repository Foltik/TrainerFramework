#include "Parser.h"

#include "AST.h"

#include <iostream>
#include <iomanip>

void Parser::expect(Parser::Type type) {
    if (currToken.type != type) {
        std::stringstream ss;
        ss << "Error: expected " << Token::strFromType(type)
           << " at L" << lastLine << ":" << lastCol
           << ", got " << Token::strFromType(currToken.type);
        throw std::runtime_error(ss.str());
    }
}

void Parser::pop(Type type) {
    expect(type);
    pop();
}

void Parser::pop() {
    lastLine = lexer.getLine();
    lastCol = lexer.getCol();
    currToken = lexer.nextToken();
}

Program Parser::program() {
    Program prog;

    while (currToken.type != Type::Eof)
        prog.blocks.push_back(block());

    return prog;
}

Block Parser::block() {
    Block block;

    pop(Type::Eol);
    block.injection = injection();
    pop(Type::Eol);

    while (currToken.type != Type::Eol && currToken.type != Type::Eof)
        block.statements.push_back(statement());

    return block;
}

Injection Parser::injection() {
    Injection inj;

    pop(Type::Lcurl);
    inj.expr = expression();
    pop(Type::Rcurl);
    pop(Type::Colon);

    return inj;
}


Expression Parser::expression() {
    Expression expr = term();

    while (currToken.type == Type::Plus || currToken.type == Type::Minus) {
        Token token = currToken;
        pop(token.type);
        expr = BinaryOp{expr, opFromToken(token), term()};
    }

    return expr;
}

Expression Parser::term() {
    Expression expr = factor();

    while (currToken.type == Type::Mul) {
        Token token = currToken;
        pop(token.type);
        expr = BinaryOp{expr, opFromToken(token), factor()};
    }

    return expr;
}

Expression Parser::factor() {
    Token token = currToken;
    Type type = token.type;

    if (type == Type::Integer) {
        return number<int>(type);
    }

    if (type == Type::Float) {
        return number<float>(type);
    }

    if (type == Type::Lbracket) {
        pop(Type::Lbracket);
        Dereference deref{expression()};
        pop(Type::Rbracket);
        return deref;
    };

    if (type == Type::Lparen) {
        pop(Type::Lparen);
        Expression expr = expression();
        pop(Type::Rparen);
        return expr;
    };

    if (type == Type::Percent) {
        pop(Type::Percent);
        Register r{string()};
        return r;
    }

    return Identifier{string()};
}

Expression Parser::variable() {
    auto id = string();
    return Identifier{id};
}

Expression Parser::reg() {
    pop(Type::Percent);
    return Register{string()};
}


Statement Parser::statement() {
    Token token = currToken;
    if (token.type == Type::Dot) {
        return directive();
    } else if (token.type == Type::Dollar) {
        return label();
    } else {
        return instruction();
    }
}

Statement Parser::label() {
    pop(Type::Dollar);

    auto str = string();
    Label label{str};

    pop(Type::Colon);
    pop(Type::Eol);
    return label;
}

Statement Parser::directive() {
    pop(Type::Dot);

    Directive directive;
    expect(Type::Id);
    directive.id.name = std::get<std::string_view>(currToken.value);
    pop();

    while (currToken.type != Type::Eol) {
        directive.args.emplace_back(expression());
        if (currToken.type == Type::Comma)
            pop(Type::Comma);
    }

    pop(Type::Eol);
    return directive;
}

Statement Parser::instruction() {
    Instruction instr;
    instr.id.name = std::get<std::string_view>(currToken.value);
    pop(Type::Id);

    while (currToken.type != Type::Eol)
        instr.args.emplace_back(expression());

    pop(Type::Eol);

    return instr;
}

std::string_view Parser::string() {
    expect(Type::Id);
    auto str = std::get<std::string_view>(currToken.value);
    pop();
    return str;
}

Operator Parser::opFromToken(const Parser::Token& token) {
    if (token.type == Type::Plus)
        return Operator::Plus;
    else if (token.type == Type::Minus)
        return Operator::Minus;
    else if (token.type == Type::Mul)
        return Operator::Mul;
    else
        return Operator::Div;
}
