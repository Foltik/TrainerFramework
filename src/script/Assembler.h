#pragma once

#include <boost/hana.hpp>

#include <cstdint>
#include <vector>

#include "AST.h"

class Assembler {
public:
    Assembler() = default;

    std::vector<uint8_t> assemble(const Program& program);

private:
    void printHier(const Program& program);
    void printExpr(const Expression& expr, int indent);
    void printStmt(const Statement& stmt, int indent);

    template <typename Variant, typename... Visitors>
    auto visit(Variant&& variant, Visitors&&... visitors) {
        return boost::apply_visitor(
            boost::hana::overload(std::forward<Visitors>(visitors)...),
            std::forward<Variant>(variant)
        );
    }
};

