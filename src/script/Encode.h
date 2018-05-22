#pragma once

#include "AST.h"

#include <cstdint>
#include <vector>

namespace Encode {
    std::vector<uint8_t> directive(const Directive& directive);
    std::vector<uint8_t> instruction(const Instruction& instruction);
}
