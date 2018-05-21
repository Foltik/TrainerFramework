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

        std::variant<std::monostate, long, float, std::string_view> value;
    };

public:
    explicit Lexer(std::string_view source);

    Token nextToken();

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
    char currChar;
};
