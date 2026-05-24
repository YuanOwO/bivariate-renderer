#include "ast/basic.hpp"

std::ostream& operator<<(std::ostream& os, const ASTPtr& node) {
    if (node) {
        node->serialize(os);
    } else {
        os << "null";
    }
    return os;
}
