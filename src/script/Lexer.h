#pragma once

#include <string_view>
#include <variant>

class Lexer {
public:
    struct Token {
        enum class Type {
            Eof,
            Eol,
            Id,
            Integer,
            Float,
            Plus,
            Minus,
            Mul,
            Div,
            Semi,
            Colon,
            Comma,
            Dot,
            Dollar,
            Percent,
            Lparen,
            Rparen,
            Lbracket,
            Rbracket,
            Lcurl,
            Rcurl
        } type;

        static std::string strFromType(Type type);

        std::variant<std::monostate, int, float, std::string_view> value;
    };

public:
    explicit Lexer(std::string_view source);

    Token nextToken();

    int getLine();
    int getCol();

private:
    using Type = Token::Type;

    void next();
    void next(int offset);
    char peekNext(int offset);

    void skipWhitespace();

    Token identifier();

    Token number();
    Token decNumber();
    Token hexNumber();
    Token flNumber();

    std::string_view src;
    std::string_view::iterator pos;

    int line = 0;
    int col = 0;

    char currChar;
};
