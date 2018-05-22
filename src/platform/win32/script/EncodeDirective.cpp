#include "../../../script/Encode.h"

#include <unordered_map>

namespace {
    enum class DirectiveType {
        DB,
        FPOP
    };

    using D = DirectiveType;
    static std::unordered_map<std::string, DirectiveType> directives = {
        {"db", D::DB},
        {"fpop", D::FPOP}
    };
}

std::vector<uint8_t> Encode::directive(const Directive& directive) {
    std::string id(directive.id.name);
    std::transform(id.begin(), id.end(), id.begin(), tolower);

    std::vector<uint8_t> bytes;

    switch(directives[id]) {
        case D::DB:
            break;
        case D::FPOP:
            break;
    }

    return bytes;
}
