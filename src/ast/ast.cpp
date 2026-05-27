#include "ast/ast.hpp"

#include "visitor/serializer.hpp"

std::ostream& operator<<(std::ostream& os, const ASTPtr& node) {
    if (node) {
        Serializer serializer(os);
        node->accept(serializer);
    } else {
        os << "null";
    }
    return os;
}
