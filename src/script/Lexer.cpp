#include "Lexer.h"

#include <cctype>
#include <string>

Lexer::Lexer(std::string_view source) {
    src = source;
    pos = source.begin();
    currChar = *pos;
}

void Lexer::next() {
    col++;
    currChar = ++pos >= src.end() ? '\0' : *pos;
}

void Lexer::next(int offset) {
    col += offset;
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
    int num = std::strtol(pos, nullptr, 16);
    while (isxdigit(currChar) || currChar == 'x')
        next();

    return Token{Type::Integer, num};
}

Lexer::Token Lexer::decNumber() {
    int num = std::strtol(pos, nullptr, 10);
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

        if (ch == '\n') {
            line++;
            col = 0;
            return {Type::Eol, {}};
        }

        if (ch == '+')
            return {Type::Plus, {}};

        if (ch == '-')
            return {Type::Minus, {}};

        if (ch == '*')
            return {Type::Mul, {}};

        if (ch == '/')
            return {Type::Div, {}};

        if (ch == ';')
            return {Type::Semi, {}};

        if (ch == ':')
            return {Type::Colon, {}};

        if (ch == ',')
            return {Type::Comma, {}};

        if (ch == '.')
            return {Type::Dot, {}};

        if (ch == '$')
            return {Type::Dollar, {}};

        if (ch == '%')
            return {Type::Percent, {}};

        if (ch == '(')
            return {Type::Lparen, {}};

        if (ch == ')')
            return {Type::Rparen, {}};

        if (ch == '[')
            return {Type::Lbracket, {}};

        if (ch == ']')
            return {Type::Rbracket, {}};

        if (ch == '{')
            return {Type::Lcurl, {}};

        if (ch == '}')
            return {Type::Rcurl, {}};
    }

    return {Type::Eof, {}};
}

int Lexer::getLine() {
    return line;
}

int Lexer::getCol() {
    return col;
}

std::string Lexer::Token::strFromType(Lexer::Type type) {
    if (type == Type::Eol)
        return "'EOL'";

    if (type == Type::Eof)
        return "'EOF'";

    if (type == Type::Plus)
        return "'+'";

    if (type == Type::Minus)
        return "'-'";

    if (type == Type::Mul)
        return "'*'";

    if (type == Type::Div)
        return "'/'";

    if (type == Type::Semi)
        return "';'";

    if (type == Type::Colon)
        return "':'";

    if (type == Type::Comma)
        return "','";

    if (type == Type::Dot)
        return "'.'";

    if (type == Type::Dollar)
        return "'$'";

    if (type == Type::Percent)
        return "'%'";

    if (type == Type::Lparen)
        return "'('";

    if (type == Type::Rparen)
        return "')'";

    if (type == Type::Lbracket)
        return "'['";

    if (type == Type::Rbracket)
        return "']'";

    if (type == Type::Lcurl)
        return "'{'";

    if (type == Type::Rcurl)
        return "'}'";

    if (type == Type::Id)
        return "Identifier";

    if (type == Type::Integer)
        return "Integer";

    return "";
}
