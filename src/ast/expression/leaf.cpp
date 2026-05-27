#include "ast/expression/leaf.hpp"

NumPtr NumberNode::clone() const {
    return new NumberNode(getLineno(), getValue());
}

IdPtr IdentifierNode::clone() const {
    return new IdentifierNode(getLineno(), getName());
}
