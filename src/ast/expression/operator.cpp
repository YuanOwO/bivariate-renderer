#include "ast/expression/operator.hpp"

#include <cmath>

#include "ast/expression/leaf.hpp"
#include "enums.hpp"
#include "errors.hpp"

//////////////////////////////////////////////////

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

//////////////////////////////////////////////////

void UnaryOpNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << getOperatorSymbol() << ", " << operand << ")";
}

bool UnaryOpNode::validate(Environment& env) {
    operand->validate(env);

    size_t op = getOperatorId();

    if (op != OP_PLUS && op != OP_MINUS) {
        throw SemanticError("Invalid unary operator.");
    }

    if (operand->hasUnknownVariable()) {
        markUnknownVariable();  // 如果子節點包含未知變數，則標記當前節點也包含未知變數
    }

    return true;
}

ExprPtr UnaryOpNode::fold(Environment& env) const {
    // 嘗試對子節點進行常量折疊
    ExprPtr folded_operand = operand->fold(env);
    size_t op = getOperatorId();

    if (op == OP_PLUS) {
        // +x => x
        return folded_operand;
    }

    if (op == OP_MINUS) {
        if (auto num_node = dynamic_cast<NumPtr>(folded_operand)) {
            // 如果子節點折疊後是一個數字節點，則可以直接計算出結果
            return new NumberNode(getLineno(), -num_node->getValue());
        }

        // -(-x) => x
        if (auto unary_node = dynamic_cast<UnaryOpNode*>(folded_operand)) {
            if (unary_node->getOperatorId() == OP_MINUS) {
                return unary_node->getOperand()->fold(env);
            }
        }

        // 否則返回一個新的 UnaryOpNode，表示 -x
        return new UnaryOpNode(getLineno(), op, folded_operand);
    }

    return new UnaryOpNode(getLineno(), op, folded_operand);
}

Value UnaryOpNode::evaluate(Environment& env) const {
    double operand_value = operand->evaluate(env);
    size_t op = getOperatorId();

    if (op == OP_PLUS) {
        return operand_value;
    }

    if (op == OP_MINUS) {
        return -operand_value;
    }

    throw SemanticError("Invalid unary operator.");
}

//////////////////////////////////////////////////

void BinaryOpNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << getOperatorSymbol() << ", " << lhs << ", " << rhs << ")";
}

bool BinaryOpNode::validate(Environment& env) {
    lhs->validate(env);
    rhs->validate(env);

    switch (getOperatorId()) {
    case OP_PLUS:
    case OP_MINUS:
    case OP_MULTIPLY:
    case OP_DIVIDE:
    case OP_EXPONENT:
        break;  // 這些都是有效的二元運算符
    default:
        throw SemanticError("Invalid binary operator.");
    }

    if (lhs->hasUnknownVariable() || rhs->hasUnknownVariable()) {
        markUnknownVariable();  // 如果任一子節點包含未知變數，則標記當前節點也包含未知變數
    }

    return true;
}

ExprPtr BinaryOpNode::fold(Environment& env) const {
    // 嘗試對左右子節點進行常量折疊
    ExprPtr folded_lhs = lhs->fold(env);
    ExprPtr folded_rhs = rhs->fold(env);

    size_t op = getOperatorId();

    // 嘗試對二元運算進行常量折疊
    auto left_number = dynamic_cast<NumberNode*>(folded_lhs);
    auto right_number = dynamic_cast<NumberNode*>(folded_rhs);

    if (left_number && right_number) {
        // 如果左右子節點都折疊成數字節點，則可以直接計算出結果
        double left_value = left_number->getValue();
        double right_value = right_number->getValue();
        double result;

        switch (op) {
        case OP_PLUS:
            result = left_value + right_value;
            break;
        case OP_MINUS:
            result = left_value - right_value;
            break;
        case OP_MULTIPLY:
            result = left_value * right_value;
            break;
        case OP_DIVIDE:
            if (right_value == 0) {
                throw SemanticError("Division by zero.");
            }
            result = left_value / right_value;
            break;
        case OP_EXPONENT:
            result = std::pow(left_value, right_value);
            break;
        default:
            throw SemanticError("Invalid binary operator.");
        }

        if (std::isinf(result) || std::isnan(result)) {
            throw SemanticError("Invalid arithmetic operation resulting in non-finite value.");
        }

        return new NumberNode(getLineno(), result);
    } else if (left_number) {
        // 如果只有左子節點折疊成數字節點，則根據運算符和左子節點的值嘗試進行簡化
        double left_value = left_number->getValue();
        // 0 + x => x
        // 0 - x => -x
        // 0 * x => 0
        // 0 / x => 0 (x != 0) -> 這裡暫時不進行折疊，因為如果 x 也折疊成 0 的話會導致除以零的錯誤
        // 0 ^ x => 0 (x > 0), 1 (x == 0), inf (x < 0) -> 這裡暫時不進行折疊，因為如果 x 也折疊成 0 的話
        //                                                 會導致不確定的結果
        // 1 * x => x
        if (left_value == 0) {
            switch (op) {
            case OP_PLUS:
                return folded_rhs;
            case OP_MINUS:
                if (auto num_node = dynamic_cast<NumPtr>(folded_rhs)) {
                    return new NumberNode(getLineno(), -num_node->getValue());
                } else {
                    return new UnaryOpNode(getLineno(), OP_MINUS, folded_rhs);
                }
            case OP_MULTIPLY:
                return new NumberNode(getLineno(), 0);
            case OP_DIVIDE:
            case OP_EXPONENT:
                break;
            }
        } else if (left_value == 1 && op == OP_MULTIPLY) {
            return folded_rhs;  // 1 * x => x
        }
    } else if (right_number) {
        // 如果只有右子節點折疊成數字節點，則根據運算符和右子節點的值嘗試進行簡化
        double right_value = right_number->getValue();
        // x + 0 => x
        // x - 0 => x
        // x * 0 => 0
        // x / 0 => Division by zero error
        // x ^ 0 => 1
        // x * 1 => x
        // x / 1 => x
        // x ^ 1 => x
        if (right_value == 0) {
            switch (op) {
            case OP_PLUS:
            case OP_MINUS:
                return folded_lhs;
            case OP_MULTIPLY:
                return new NumberNode(getLineno(), 0);
            case OP_DIVIDE:
                throw SemanticError("Division by zero.");
            case OP_EXPONENT:
                return new NumberNode(getLineno(), 1);
            }
        } else if (right_value == 1 && (op == OP_MULTIPLY || op == OP_DIVIDE || op == OP_EXPONENT)) {
            return folded_lhs;
        }
    }

    // 否則返回一個新的 BinaryOpNode，表示折疊後的二元運算
    return new BinaryOpNode(getLineno(), op, folded_lhs, folded_rhs);
}

Value BinaryOpNode::evaluate(Environment& env) const {
    double left_value = lhs->evaluate(env);
    double right_value = rhs->evaluate(env);
    size_t op = getOperatorId();

    switch (op) {
    case OP_PLUS:
        return left_value + right_value;
    case OP_MINUS:
        return left_value - right_value;
    case OP_MULTIPLY:
        return left_value * right_value;
    case OP_DIVIDE:
        if (right_value == 0) {
            throw RuntimeError("Division by zero.");
        }
        return left_value / right_value;
    case OP_EXPONENT:
        return std::pow(left_value, right_value);
    default:
        throw SemanticError("Invalid binary operator.");
    }
}
