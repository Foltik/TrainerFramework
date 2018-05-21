#include "Lexer.h"

#include <cctype>
#include <string>

Lexer::Lexer(std::string_view source) {
    src = source;
    pos = source.begin();
    currChar = *pos;
}

void Lexer::next() {
    currChar = ++pos >= src.end() ? '\0' : *pos;
}

void Lexer::next(int offset) {
    pos = pos + offset;
    currChar = pos >= src.end() ? '\0' : *pos;
}

char Lexer::peekNext(int offset) {
    auto newpos = pos + offset;
    return newpos >= src.end() ? '\0' : *newpos;
}

void Lexer::skipWhitespace() {
    while (currChar && std::isspace(currChar) && currChar != '\n')
        next();
}

Lexer::Token Lexer::identifier() {
    auto begin = pos;
    while (currChar && std::isalnum(currChar))
        next();

    std::string_view identifier(begin, pos - begin);

    return Token{Type::Id, identifier};
}

Lexer::Token Lexer::number() {
    if (currChar == '0' && peekNext(1) == 'x')
        return hexNumber();

    if (currChar == '0' && peekNext(1) == 'f')
        return flNumber();

    return decNumber();
}

Lexer::Token Lexer::hexNumber() {
    long num = std::strtol(pos, nullptr, 16);
    while (isxdigit(currChar) || currChar == 'x')
        next();

    return Token{Type::Integer, num};
}

Lexer::Token Lexer::decNumber() {
    long num = std::strtol(pos, nullptr, 10);
    while (isdigit(currChar))
        next();

    return Token{Type::Integer, num};
}

Lexer::Token Lexer::flNumber() {
    next(2);
    float num = std::strtof(pos, nullptr);

    while (isdigit(currChar) || currChar == '.')
        next();

    return Token{Type::Float, num};
}

Lexer::Token Lexer::nextToken() {
    while (currChar != '\0') {
        if (std::isspace(currChar))
            skipWhitespace();

        if (std::isalpha(currChar))
            return identifier();

        if (std::isdigit(currChar))
            return number();

        char ch = currChar;
        next();

        if (ch == '\n')
            return {Type::Eol};

        if (ch == '+')
            return {Type::Plus};

        if (ch == '-')
            return {Type::Minus};

        if (ch == '*')
            return {Type::Mul};

        if (ch == '/')
            return {Type::Div};

        if (ch == ';')
            return {Type::Semi};

        if (ch == ':')
            return {Type::Colon};

        if (ch == ',')
            return {Type::Comma};

        if (ch == '.')
            return {Type::Dot};

        if (ch == '$')
            return {Type::Dollar};

        if (ch == '(')
            return {Type::Lparen};

        if (ch == ')')
            return {Type::Rparen};

        if (ch == '[')
            return {Type::Lbracket};

        if (ch == ']')
            return {Type::Rbracket};

        if (ch == '{')
            return {Type::Lcurl};

        if (ch == '}')
            return {Type::Rcurl};
    }

    return {Type::Eof};
}
