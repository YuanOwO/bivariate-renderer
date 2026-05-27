#include "visitor/validator.hpp"

#include "ast.hpp"
#include "errors.hpp"

void* Validator::visit(const NumberNode* node) {
    // 數字節點總是有效的
    return nullptr;
}

void* Validator::visit(const IdentifierNode* node) {
    // 檢查變數是否已定義
    auto name = node->getName();
    bool is_known = env->hasVariable(name);
    bool is_unknown = env->isUnknownVariable(name);

    // 變數未定義且未標記為未知，報告語意錯誤
    if (!is_known && !is_unknown) {
        throw SemanticError("Undefined variable: " + name);
    }

    if (is_unknown) {
        has_unknown_variables = true;  // 記錄遇到過未知數
    }

    return nullptr;
}

void* Validator::visit(const UnaryOpNode* node) {
    auto op = node->getOperatorId();

    if (op != OP_PLUS && op != OP_MINUS) {
        throw SemanticError("Invalid unary operator: " + String(node->getOperatorSymbol()));
    }

    // 驗證子節點
    node->getOperand()->accept(*this);

    return nullptr;
}

void* Validator::visit(const BinaryOpNode* node) {
    auto op = node->getOperatorId();

    if (op != OP_PLUS && op != OP_MINUS && op != OP_MULTIPLY && op != OP_DIVIDE && op != OP_EXPONENT) {
        throw SemanticError("Invalid binary operator: " + String(node->getOperatorSymbol()));
    }

    // 驗證左右子節點
    node->getLeftOperand()->accept(*this);
    node->getRightOperand()->accept(*this);

    return nullptr;
}

void* Validator::visit(const FuncCallNode* node) {
    auto func_name = node->getFuncId()->getName();

    if (!env->hasFunction(func_name)) {
        throw SemanticError("Undefined function: " + func_name);
    }

    Function func = env->getFunction(func_name);

    if (node->getArguments().size() != func.parameters.size()) {
        throw SemanticError("Function " + func_name + " expects " + std::to_string(func.parameters.size()) +
                            " arguments, but got " + std::to_string(node->getArguments().size()) + ".");
    }

    // 驗證參數
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
    }

    return nullptr;
}

void* Validator::visit(const AssignmentNode* node) {
    auto var_name = node->getId()->getName();

    if (env->isDefined(var_name)) {
        throw SemanticError("Variable name '" + var_name + "' is already defined.");
    }

    // 驗證右側表達式
    node->getValue()->accept(*this);

    if (has_unknown_variables) {
        throw SemanticError("Cannot assign to variable '" + var_name +
                            "' because the assigned expression contains unknown variables.");
    }

    return nullptr;
}

void* Validator::visit(const FuncDefNode* node) {
    auto func_name = node->getId()->getName();

    if (env->isDefined(func_name)) {
        throw SemanticError("Function name '" + func_name + "' is already defined.");
    }

    Vector<String> param_names;
    for (const auto& param : node->getParameters()) {
        auto param_name = param->getName();
        if (std::find(param_names.begin(), param_names.end(), param_name) != param_names.end()) {
            throw SemanticError("Duplicate parameter name '" + param_name + "' in function '" + func_name +
                                "'.");
        }
        param_names.push_back(param_name);
    }

    Environment func_env(param_names, env);
    Environment* bak = env;  // 保存當前環境指針
    env = &func_env;         // 切換到函數環境

    // 驗證函數體
    node->getBody()->accept(*this);

    env = bak;  // 恢復原環境指針

    return nullptr;
}

void* Validator::visit(const PlotNode* node) {
    // 驗證表達式
    node->getExpression()->accept(*this);

    return nullptr;
}
