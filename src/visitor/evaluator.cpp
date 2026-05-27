#include "visitor/evaluator.hpp"

#include <cmath>
#include <limits>

#include "ast.hpp"
#include "errors.hpp"
#include "plot.hpp"
#include "utils.hpp"

void* Evaluator::visit(const NumberNode* node) {
    return_value = node->getValue();
    return nullptr;
}

void* Evaluator::visit(const IdentifierNode* node) {
    auto name = node->getName();
    return_value = env->getVariable(name);

    return nullptr;
}

void* Evaluator::visit(const UnaryOpNode* node) {
    // 先計算子節點的值
    node->getOperand()->accept(*this);
    Value operand_value = return_value;

    if (node->getOperatorId() == OP_MINUS) {
        return_value = -return_value;
    }

    // OP_PLUS 不需要特別處理，因為它不改變值

    return nullptr;
}

void* Evaluator::visit(const BinaryOpNode* node) {
    // 先計算左子節點的值
    node->getLeftOperand()->accept(*this);
    Value left_value = return_value;

    // 再計算右子節點的值
    node->getRightOperand()->accept(*this);
    Value right_value = return_value;

    switch (node->getOperatorId()) {
    case OP_PLUS:
        return_value = left_value + right_value;
        break;
    case OP_MINUS:
        return_value = left_value - right_value;
        break;
    case OP_MULTIPLY:
        return_value = left_value * right_value;
        break;
    case OP_DIVIDE:
        if (right_value == 0) {
            throw RuntimeError("Division by zero");
        }
        return_value = left_value / right_value;
        break;
    case OP_EXPONENT:
        return_value = std::pow(left_value, right_value);
        break;
    default:
        throw RuntimeError("Unknown operator");
    }

    return nullptr;
}

void* Evaluator::visit(const FuncCallNode* node) {
    auto func_name = node->getFuncId()->getName();
    Function func = env->getFunction(func_name);

    std::vector<Value> evaluated_args;
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
        evaluated_args.push_back(return_value);
    }

    if (func.is_builtin()) {
        // 對於內建函數，直接調用其函數指標
        Environment call_env;  // 為內建函數創建一個臨時環境
        for (size_t i = 0; i < func.parameters.size(); i++) {
            call_env.setVariable(func.parameters[i], evaluated_args[i]);
        }
        return_value = func.builtin_func(call_env);
    } else {
        // 對於用戶定義的函數，創建一個新的環境並評估函數體
        Environment call_env(func.parameters, env);  // 創建一個新的環境，外部環境為當前環境
        for (size_t i = 0; i < func.parameters.size(); i++) {
            call_env.setVariable(func.parameters[i], evaluated_args[i]);
        }

        Evaluator func_evaluator(&call_env);
        func.body->accept(func_evaluator);
        return_value = func_evaluator.return_value;
    }

    return nullptr;
}

void* Evaluator::visit(const AssignmentNode* node) {
    auto var_name = node->getId()->getName();
    node->getValue()->accept(*this);
    env->setVariable(var_name, return_value);

    return nullptr;
}

void* Evaluator::visit(const FuncDefNode* node) {
    auto func_name = node->getId()->getName();
    std::vector<String> params;
    for (const auto& param : node->getParameters()) {
        params.push_back(param->getName());
    }
    env->setFunction(func_name, params, node->getBody()->clone());

    return nullptr;
}

void* Evaluator::visit(const PlotNode* node) {
    // 目前暫不實現具體的繪圖功能
    // 這裡可以添加對繪圖庫的調用，將計算結果渲染成圖片

    int resolution = 800;
    Value min_x = -10, max_x = 10;
    Value min_y = -10, max_y = 10;
    Value step_x = (max_x - min_x) / resolution;
    Value step_y = (max_y - min_y) / resolution;

    Environment plot_env(env);  // 為繪圖創建一個新的環境，外部環境為當前環境
    Evaluator plot_evaluator(&plot_env);

    std::vector<Value> data(resolution * resolution);
    Value z_min = std::numeric_limits<Value>::max();
    Value z_max = std::numeric_limits<Value>::lowest();

    for (size_t j = 0; j < resolution; j++) {
        Value y = max_y - j * step_y;
        for (size_t i = 0; i < resolution; i++) {
            Value x = min_x + i * step_x;

            plot_env.setVariable("x", x);
            plot_env.setVariable("y", y);
            node->getExpression()->accept(plot_evaluator);
            Value z = plot_evaluator.return_value;
            if (std::isfinite(z)) {
                z_min = std::min(z_min, z);
                z_max = std::max(z_max, z);
            }
            data[j * resolution + i] = z;
        }
    }

    // normalize data to [0, 1] range for better color mapping
    for (auto& z : data) {
        if (std::isfinite(z)) {
            z = normalize(z, z_min, z_max);
        } else if (std::isinf(z)) {
            z = z > 0 ? 1.0 : 0.0;  // 正無限大映射到 1，負無限大映射到 0
        } else {
            z = 0.5;  // 對於 NaN，使用中間值
        }
    }

    renderPlot(node->getFilename(), data, resolution);

    return nullptr;
}
