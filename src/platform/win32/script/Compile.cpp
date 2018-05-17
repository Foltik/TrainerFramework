#include "../../../script/Script.h"

#include <iostream>
#include <algorithm>
#include <map>

namespace {
    struct Label {
        uintptr_t address;
    };

    struct Insertion {
        uintptr_t address;
    };

    struct Jump {
        std::string target;
        uintptr_t offset;
    };

    std::map<std::string, uint8_t> regs = {
            {"eax", 0},
            {"ecx", 1},
            {"edx", 2},
            {"ebx", 3},
            {"esp", 4},
            {"ebp", 5},
            {"esi", 6},
            {"edi", 7}
    };
}

namespace {
    unsigned long parseNum(const std::string& str) {
        if (str.substr(0, 2) == "0x") {
            return std::stoul(str.substr(2), 0, 16);
        } else if (str.find('.') != std::string::npos) {
            float fl = std::stof(str);
            return reinterpret_cast<unsigned long&>(fl);
        } else {
            return std::stoul(str);
        }
    }
}

void Script::compile() {
    std::vector<std::vector<std::string>> source;

    for (auto& line : getTokens(code.begin(), "\n")) {
        std::cout << "LINE(" << line.size() << "): \"" << line << "\"" << std::endl;

        std::vector<std::string> tokens = getTokens(line.begin(), ", ");

        for (auto& token : tokens)
            std::cout << "TOKEN: \"" << token << "\"" << std::endl;

        source.push_back(tokens);
        std::cout << std::endl;
    }

    std::map<std::string, Label> labels;
    std::map<std::string, Insertion> insertions;
    std::map<std::string, std::vector<uint8_t>> insertionCode;
    std::vector<Jump> jumps;

    for (auto& varpair : vars) {
        std::cout << "VAR \"" << varpair.first << "\"" << ": 0x" << std::hex << varpair.second << std::endl;
    }

    std::string currInsertion = "";
    uintptr_t currOffset = 0;
    for (const auto& tokens : source) {
        for (auto tok = tokens.begin(); tok != tokens.end(); tok++) {
            std::cout << "Parsing \"" << *tok << "\" (0x" << std::hex << currOffset << ")" << std::endl;
            if ((*tok)[0] == '{') {
                std::string expr = tok->substr(1, tok->length() - 3);
                uintptr_t location = 0;
                if (std::isdigit(expr[expr.length() - 1])) {
                    std::string ops = "+-";
                    unsigned long op = static_cast<unsigned long>(std::find_first_of(expr.begin(), expr.end(),
                                                                                     ops.begin(), ops.end()) -
                                                                  expr.begin());
                    location = vars[expr.substr(0, op)];
                    if (expr[op] == '+')
                        location += parseNum(expr.substr(op + 1));
                    else if (expr[op] == '-')
                        location -= parseNum(expr.substr(op + 1));
                } else {
                    location = vars[expr];
                }
                insertions.insert({expr, {location}});
                currInsertion = expr;
                currOffset = 0;
            } else if ((*tok)[tok->length() - 1] == ':') {
                labels.insert({tok->substr(0, tok->length() - 1), {insertions[currInsertion].address + currOffset}});
            } else if (*tok == "jmp") {
                tok++;
                if (std::isdigit((*tok)[0])) {
                    // rel = dest - (src + offset)
                    // rel = dest - offset - src
                    // subtract src later to get offset
                    jumps.push_back({"", parseNum(*tok) - currOffset});
                } else {
                    jumps.push_back({*tok, 0});
                }
                currOffset += 0x5;
            } else if (*tok == "fpop") {
                currOffset += 0x2;
            } else if (*tok == "fld") {
                tok++;
                currOffset += 0x6;
            } else if (*tok == "pop") {
                tok++;
                currOffset += 0x1;
            } else if (*tok == "db") {
                int count = tokens.end() - tok - 1;
                tok += count;
                currOffset += count * 0x1;
                //std::cout << (tok - tokens.begin()) << " " << (tokens.end() - tokens.begin()) << std::endl;
                //currOffset += 0x1 * (tokens.end() - tok - 1);
            } else if (*tok == "dfl") {
                int count = tokens.end() - tok - 1;
                tok += count;
                currOffset += count * 0x4;
            }
        }
    }

    for (auto& ins : insertions) {
        std::cout << "Insertion \"" << ins.first << "\" (0x" << std::hex << ins.second.address << ")" << std::endl;
    }

    for (auto& label : labels) {
        std::cout << "Label \"" << label.first << "\" (+0x" << std::hex << label.second.address << ")" << std::endl;
    }

    for (auto& jump : jumps) {
        std::cout << "Jump to \"" << jump.target << "\" (dest - offset 0x" << std::hex << jump.offset << ")"
                  << std::endl;
    }

    std::cout << "PASS 2" << std::endl;
    currOffset = 0;
    currInsertion = "";
    for (const auto& tokens : source) {
        for (auto tok = tokens.begin(); tok != tokens.end(); tok++) {
            std::cout << "Encoding \"" << *tok << "\"" << " (0x" << std::hex << currOffset << ")" << std::endl;
            if ((*tok)[0] == '{') {
                std::string expr = tok->substr(1, tok->length() - 3);
                currInsertion = expr;
                currOffset = 0;
            } else if (*tok == "jmp") {
                tok++;
                insertionCode[currInsertion].push_back(0xE9);
                uintptr_t dest;
                if (std::isdigit((*tok)[0])) {
                    dest = parseNum(*tok);
                } else if (labels.count(*tok)) {
                    dest = labels[*tok].address;
                } else {
                    dest = insertions[*tok].address;
                }
                uintptr_t jmpSrc = (insertions[currInsertion].address + currOffset + 0x5);
                uintptr_t relativeOffset = dest - jmpSrc;
                std::cout << "addr: 0x" << std::hex << insertions[currInsertion].address << std::endl;
                std::cout << "currOffset: 0x" << std::hex << currOffset << " +0x5" << std::endl;
                std::cout << "src: 0x" << std::hex << jmpSrc << std::endl;
                std::cout << "dest: 0x" << std::hex << dest << std::endl;
                std::cout << "offset: 0x" << std::hex << relativeOffset << std::endl;
                for (int i = 0; i < 4; i++)
                    insertionCode[currInsertion].push_back(((uint8_t*)(&relativeOffset))[i]);
                currOffset += 0x5;

            } else if (*tok == "fpop") {
                insertionCode[currInsertion].push_back(0xDD);
                insertionCode[currInsertion].push_back(0xD8);
                currOffset += 0x2;
            } else if (*tok == "fld") {
                tok++;
                insertionCode[currInsertion].push_back(0xD9);
                uintptr_t arg;
                if ((*tok)[0] == '[') {
                    std::string expr = tok->substr(1, tok->length() - 2);
                    if (labels.count(expr)) {
                        arg = labels[expr].address;
                    } else if (insertions.count(expr)) {
                        arg = insertions[expr].address;
                    }
                    insertionCode[currInsertion].push_back(0x05);
                    for (int i = 0; i < 4; i++)
                        insertionCode[currInsertion].push_back(((uint8_t*)(&arg))[i]);
                }
                currOffset += 0x6;
            } else if (*tok == "pop") {
                tok++;
                insertionCode[currInsertion].push_back(static_cast<uint8_t&&>(0x58 + regs[*tok]));
                currOffset += 0x1;
            } else if (*tok == "db") {
                int count = tokens.end() - tok - 1;
                for (int i = 0; i < count; i++) {
                    tok++;
                    insertionCode[currInsertion].push_back(static_cast<uint8_t>(parseNum(*tok)));
                    currOffset++;
                }
            } else if (*tok == "dfl") {
                int count = tokens.end() - tok - 1;
                for (int i = 0; i < count; i++) {
                    tok++;
                    auto num = parseNum(*tok);
                    for (int j = 0; j < 4; j++)
                        insertionCode[currInsertion].push_back(((uint8_t*)(&num))[j]);
                    currOffset += 0x4;
                }
            }
        }
    }

    for (const auto& pair : insertionCode) {
        std::cout << std::endl << "\"" << pair.first << "\" (0x" << std::hex << insertions[pair.first].address << ")" << std::endl;
        for (long byte : pair.second) {
            std::cout << std::hex << byte << " ";
        }
        std::cout << std::endl;
    }

    for (const auto& ins : insertions)
        bytecode.insert({ins.second.address, insertionCode[ins.first]});
}
