#include "Assembler.h"

#include "AST.h"
#include "Encode.h"

#include <iostream>

void Assembler::error(const std::string& msg) {
    throw std::runtime_error(msg);
}

Assembler::InjectionList Assembler::assemble(const Program& program, const std::map<std::string, uintptr_t>& variables) {
    printHier(program);
    symbolTable = variables;

    std::vector<std::pair<std::string, std::vector<uint8_t>::iterator>> patchList;

    InjectionList bytecode;
    for (const auto& block : program.blocks) {
        uintptr_t injectionPoint = evalExpr(block.injection.expr);
        auto pair = std::make_pair(injectionPoint, std::vector<uint8_t>{});
        auto& bytes = pair.second;

        uintptr_t byteOffset = 0;
        for (const auto& statement : block.statements) {
            visit(statement,
                [&](const Label& label) {
                    symbolTable.insert({std::string(label.id.name), injectionPoint + byteOffset});
                },
                [&](const Directive& dir) {
                    auto res = Encode::directive(dir);
                    bytes.insert(bytes.begin(), res.begin(), res.end());
                    byteOffset += res.size();
                },
                [&](const Instruction& instr) {
                    auto res = Encode::instruction(instr);
                    bytes.insert(bytes.begin(), res.begin(), res.end());
                    byteOffset += res.size();
                }
            );
        }

        bytecode.push_back(pair);
    }

    for (const auto& pair : patchList) {
        auto id = pair.first;
        auto it = pair.second;

        if (!symbolTable.count(id))
            error("Unknown symbol: " + id);

        for (int i = 0; i < 4; i++)
            *(it++) = static_cast<uint8_t>(symbolTable[id] >> ((3 - i) * 8));
    }

    return bytecode;
}

uintptr_t Assembler::evalExpr(const Expression& expr) {
    return static_cast<uintptr_t>(visit(expr,
        [this](const Identifier& id) {
            int res = 0;

            std::string str(id.name);
            if (!symbolTable.count(str))
                error("Failed to find symbol " + str);
            else
                res = symbolTable[str];

            return res;
        },
        [this](const Number& num) {
            int n;
            visit(num.value,
                [&n](int i) {
                    n = i;
                },
                [this](float) {
                    error("Unexpected float");
                }
            );

            return n;
        },
        [this](const BinaryOp& op) {
            int res = 0;

            if (op.op == Operator::Plus)
                res = evalExpr(op.left) + evalExpr(op.right);
            else if (op.op == Operator::Minus)
                res = evalExpr(op.left) - evalExpr(op.right);
            else if (op.op == Operator::Mul)
                res = evalExpr(op.left) * evalExpr(op.right);
            else
                res = evalExpr(op.left) / evalExpr(op.right);

            return res;
        },
        [this](const Register&) {
            error("Unexpected register");
            return 0;
        },
        [this](const Dereference&) {
            error("Unexpected dereference");
            return 0;
        }
    ));
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
            [&](const Label& label) {
                printIndent(indent);
                std::cout << "Label: " << label.id.name << std::endl;
            },
            [&](const Directive& dir) {
                printIndent(indent);
                std::cout << "Directive: " << dir.id.name << std::endl;
                for (auto& arg : dir.args)
                    printExpr(arg, indent + 1);
            },
            [&](const Instruction& instr) {
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
            [&](const Identifier& x) {
                printIndent(indent);
                std::cout << "Identifier: " << x.name << std::endl;
            },
            [&](const Register& r) {
                printIndent(indent);
                std::cout << "Register: " << r.name << std::endl;
            },
            [&](const Number& x) {
                printIndent(indent);
                std::cout << "Number: ";
                visit(x.value,
                      [](float f) { std::cout << "Float " << f; },
                      [](int n) { std::cout << "Long " << n; }
                );
                std::cout << std::endl;
            },
            [&](const BinaryOp& x) {
                printIndent(indent);
                std::cout << "BinaryOp: " << std::endl;
                printExpr(x.left, indent + 1);
                printExpr(x.right, indent + 1);
            },
            [&](const Dereference& x) {
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

        for (auto& statement : block.statements)
            printStmt(statement, 1);

        std::cout << std::endl;
    }
}
