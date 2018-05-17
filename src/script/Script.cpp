#include "Script.h"

#include "../mem/Memory.h"
#include "../mem/Process.h"

Script::Script(const std::string& sourceCode, const std::map<std::string, uintptr_t>& variables) {
    code = sourceCode;
    vars = variables;
    compile();
}

void Script::execute(const Process& p) {
    for (auto& pair : bytecode)
        Mem::writeBytes(p, pair.first, pair.second);
}

std::vector<std::string> Script::getTokens(std::string::const_iterator str, const std::string& delim) {
    std::vector<std::string> res;
    do {
        auto begin = str;
        while (delim.find(*str) == std::string::npos && *str)
            str++;

        if (str != begin)
            res.push_back(std::string(begin, str));
    } while (0 != *str++);
    return res;
};
