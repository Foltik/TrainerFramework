#pragma once

#include <boost/hana.hpp>

#include <cstdint>
#include <vector>

#include "AST.h"

class Assembler {
public:
    Assembler() = default;

    using InjectionList = std::vector<std::pair<uintptr_t, std::vector<uint8_t>>>;
    InjectionList assemble(const Program& program, const std::map<std::string, uintptr_t>& variables);

private:
    void error(const std::string& msg);

    void printHier(const Program& program);
    void printExpr(const Expression& expr, int indent);
    void printStmt(const Statement& stmt, int indent);

    uintptr_t evalExpr(const Expression& expr);

    template <typename Variant, typename... Visitors>
    auto visit(Variant&& variant, Visitors&&... visitors) {
        return boost::apply_visitor(
            boost::hana::overload(std::forward<Visitors>(visitors)...),
            std::forward<Variant>(variant)
        );
    }

    std::map<std::string, uintptr_t> symbolTable;
};

