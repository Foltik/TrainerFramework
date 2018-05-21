#pragma once

#include <boost/variant.hpp>

#include <vector>

struct Identifier;
struct Register;
struct Number;
struct BinaryOp;
struct UnaryOp;
struct Dereference;

using Expression = boost::variant<
        Identifier,
        Register,
        Number,
        boost::recursive_wrapper<BinaryOp>,
        boost::recursive_wrapper<UnaryOp>,
        boost::recursive_wrapper<Dereference>>;

struct Identifier {
    std::string_view name;
};

struct Register {
    std::string_view name;
};

struct Number {
    boost::variant<long, float> value;
};

enum class Operator {
    Plus,
    Minus,
    Mul,
    Div
};

struct BinaryOp {
    Expression left;
    Operator op;
    Expression right;
};

struct UnaryOp {
    Operator op;
    Expression operand;
};

struct Dereference {
    Expression operand;
};



struct Label;
struct Directive;
struct Instruction;

using Statement = boost::variant<
        Label,
        Directive,
        Instruction>;

struct Label {
    Identifier id;
};

struct Instruction {
    Identifier id;
    std::vector<Expression> args;
};

struct Directive {
    Identifier id;
    std::vector<Expression> args;
};



struct Injection {
    Expression expr;
};

struct Block {
    Injection injection;
    std::vector<Statement> statements;
};

struct Program {
    std::vector<Block> blocks;
};
