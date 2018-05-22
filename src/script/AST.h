#pragma once

#include <boost/variant.hpp>
#include <boost/hana.hpp>
#include <vector>

template<typename Variant, typename... Overloads>
auto visit(Variant&& variant, Overloads&& ... overloads) {
    return boost::apply_visitor(
        boost::hana::overload(std::forward<Overloads>(overloads)...),
        std::forward<Variant>(variant)
    );
}

struct Identifier;
struct Register;
struct Number;
struct BinaryOp;
struct Dereference;

using Expression = boost::variant<
    Identifier,
    Register,
    Number,
    boost::recursive_wrapper<BinaryOp>,
    boost::recursive_wrapper<Dereference>>;

struct Identifier {
    std::string_view name;
};

struct Register {
    std::string_view name;
};

struct Number {
    boost::variant<int, float> value;
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
