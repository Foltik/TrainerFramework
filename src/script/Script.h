#pragma once

#include <string>
#include <vector>
#include <map>

class Process;

class Script {
public:
    Script(const std::string& sourceCode, const std::map<std::string, uintptr_t>& vars);

    void execute(const Process& p);

private:
    void compile();

    static std::vector<std::string> getTokens(std::string::const_iterator str, const std::string& delim);

    std::string code;
    std::map<std::string, uintptr_t> vars;
    std::map<uintptr_t, std::vector<uint8_t>> bytecode;
};
