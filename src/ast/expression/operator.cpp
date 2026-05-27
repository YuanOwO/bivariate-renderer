#include "ast/expression/operator.hpp"

const char* OperatorNode::getOperatorSymbol() const {
    switch (op) {
    case OP_PLUS:
        return "+";
    case OP_MINUS:
        return "-";
    case OP_MULTIPLY:
        return "*";
    case OP_DIVIDE:
        return "/";
    case OP_EXPONENT:
        return "^";
    default:
        return "unknown_operator";
    }
}

UnaryOpNode* UnaryOpNode::clone() const {
    ExprPtr cloned_operand = operand->clone();
    return new UnaryOpNode(getLineno(), getOperatorId(), cloned_operand);
}

BinaryOpNode* BinaryOpNode::clone() const {
    ExprPtr cloned_lhs = lhs->clone();
    ExprPtr cloned_rhs = rhs->clone();
    return new BinaryOpNode(getLineno(), getOperatorId(), cloned_lhs, cloned_rhs);
}
