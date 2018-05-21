#include <iostream>
#include "Assembler.h"

#include "AST.h"

std::vector<uint8_t> Assembler::assemble(const Program& program) {
    printHier(program);
    return std::vector<uint8_t>();
}

namespace {
    void printIndent(int n) {
        for (int i = 0; i < n; i++)
            std::cout << "   ";
    }
}

void Assembler::printStmt(const Statement& stmt, int indent) {
    visit(
        stmt,
        [&](Label label) {
            printIndent(indent);
            std::cout << "Label: " << label.id.name << std::endl;
        },
        [&](Directive dir) {
            printIndent(indent);
            std::cout << "Directive: " << dir.id.name << std::endl;
            for(auto& arg : dir.args)
                printExpr(arg, indent + 1);
        },
        [&](Instruction instr) {
            printIndent(indent);
            std::cout << "Instruction: " << instr.id.name << std::endl;
            for (auto& arg : instr.args)
                printExpr(arg, indent + 1);
        }
    );
}

void Assembler::printExpr(const Expression& expr, int indent) {
    visit(
        expr,
        [&](Identifier x) {
            printIndent(indent);
            std::cout << "Identifier: " << x.name << std::endl;
        },
        [&](Register r) {
            printIndent(indent);
            std::cout << "Register: " << r.name << std::endl;
        },
        [&](Number x) {
            printIndent(indent);
            std::cout << "Number: ";
            visit(x.value,
                [](float f) { std::cout << "Float " << f; },
                [](long n) { std::cout << "Long " << n; }
            );
            std::cout << std::endl;
        },
        [&](BinaryOp x) {
            printIndent(indent);
            std::cout << "BinaryOp: " << std::endl;
            printExpr(x.left, indent + 1);
            printExpr(x.right, indent + 1);
        },
        [&](UnaryOp x) {
            printIndent(indent);
            std::cout << "UnaryOp: " << std::endl;
            printExpr(x.operand, indent + 1);
        },
        [&](Dereference x) {
            printIndent(indent);
            std::cout << "Dereference" << std::endl;
            printExpr(x.operand, indent + 1);
        }
    );
}

void Assembler::printHier(const Program& program) {
    std::cout << "Program" << std::endl;
    for (const Block& block : program.blocks) {
        printIndent(0);
        std::cout << "Block" << std::endl;

        printIndent(1);
        std::cout << "Injection" << std::endl;
        printExpr(block.injection.expr, 2);

        for(auto& statement : block.statements)
            printStmt(statement, 1);

        std::cout << std::endl;
    }
}
