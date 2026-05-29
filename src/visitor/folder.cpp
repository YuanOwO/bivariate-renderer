#include "visitor/folder.hpp"

#include <cmath>

#include "ast.hpp"
#include "errors.hpp"

void* ConstantFolder::visit(const NumberNode* node) {
    // 數字節點不需要折疊，直接返回自身
    return node->clone();
}

void* ConstantFolder::visit(const IdentifierNode* node) {
    // 變數節點無法折疊，直接返回自身
    return node->clone();
}

void* ConstantFolder::visit(const UnaryOpNode* node) {
    // 先折疊子節點
    ExprPtr operand = static_cast<ExprPtr>(fold(node->getOperand()));
    auto op = node->getOperatorId();

    // +x => x
    if (op == OP_PLUS) {
        return operand;  // 直接返回子節點，因為 +x 等價於 x
    }

    if (op == OP_MINUS) {
        // 如果子節點是數字，直接計算結果
        if (auto num_node = dynamic_cast<NumberNode*>(operand)) {
            return new NumberNode(node->getLineno(), -num_node->getValue());
        }
        // -(-x) => x
        if (auto unary_node = dynamic_cast<UnaryOpNode*>(operand)) {
            if (unary_node->getOperatorId() == OP_MINUS) {
                return unary_node->getOperand()->clone();  // 返回子節點的克隆
            }
        }
    }

    // 無法折疊，返回新的 UnaryOpNode
    return new UnaryOpNode(node->getLineno(), op, operand);
}

void* ConstantFolder::visit(const BinaryOpNode* node) {
    // 先折疊左右子節點
    ExprPtr left = static_cast<ExprPtr>(fold(node->getLeftOperand()));
    ExprPtr right = static_cast<ExprPtr>(fold(node->getRightOperand()));
    auto op = node->getOperatorId();

    auto left_num = dynamic_cast<NumberNode*>(left);
    auto right_num = dynamic_cast<NumberNode*>(right);

    // 如果左右子節點都是數字，直接計算結果
    if (left_num && right_num) {
        double result;
        switch (op) {
        case OP_PLUS:
            result = left_num->getValue() + right_num->getValue();
            break;
        case OP_MINUS:
            result = left_num->getValue() - right_num->getValue();
            break;
        case OP_MULTIPLY:
            result = left_num->getValue() * right_num->getValue();
            break;
        case OP_DIVIDE:
            if (right_num->getValue() == 0) {
                throw SemanticError("Division by zero in constant folding");
            }
            result = left_num->getValue() / right_num->getValue();
            break;
        case OP_EXPONENT:
            result = std::pow(left_num->getValue(), right_num->getValue());
            break;
        }
        return new NumberNode(node->getLineno(), result);
    }

    // 無法折疊，返回新的 BinaryOpNode
    return new BinaryOpNode(node->getLineno(), op, left, right);
}

void* ConstantFolder::visit(const FuncCallNode* node) {
    // 先折疊函數名稱和參數
    IdPtr func_id = node->getFuncId()->clone();
    Vector<ExprPtr>* args = new Vector<ExprPtr>();
    for (const auto& arg : node->getArguments()) {
        args->push_back(static_cast<ExprPtr>(fold(arg)));
    }
    return new FuncCallNode(node->getLineno(), func_id, args);
}

void* ConstantFolder::visit(const AssignmentNode* node) {
    ExprPtr value = static_cast<ExprPtr>(fold(node->getValue()));
    return new AssignmentNode(node->getLineno(), node->getId()->clone(), value);
}

void* ConstantFolder::visit(const FuncDefNode* node) {
    ExprPtr body = static_cast<ExprPtr>(fold(node->getBody()));
    Vector<IdPtr>* params = new Vector<IdPtr>();
    for (const auto& param : node->getParameters()) {
        params->push_back(param->clone());
    }
    return new FuncDefNode(node->getLineno(), node->getId()->clone(), params, body);
}

void* ConstantFolder::visit(const PlotNode* node) {
    ExprPtr expr = static_cast<ExprPtr>(fold(node->getExpression()));
    return new PlotNode(node->getLineno(), expr, node->getFilename());
}