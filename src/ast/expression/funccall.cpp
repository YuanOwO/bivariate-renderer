#include "ast/expression/funccall.hpp"

#include "enums.hpp"
#include "env.hpp"
#include "errors.hpp"

void FuncCallNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << func_name->getName() << ", [";
    for (size_t i = 0; i < arguments.size(); i++) {
        os << arguments[i];
        if (i < arguments.size() - 1) {
            os << ", ";
        }
    }
    os << "])";
}

bool FuncCallNode::validate(Environment& env) {
    std::string func_name_str = func_name->getName();

    if (!env.hasFunction(func_name_str)) {
        throw SemanticError("Undefined function '" + func_name_str + "'.");
    }

    Function func = env.getFunction(func_name_str);

    if (arguments.size() != func.parameters.size()) {
        throw SemanticError("Function '" + func_name_str + "' expects " +
                            std::to_string(func.parameters.size()) + " arguments, but got " +
                            std::to_string(arguments.size()) + ".");
    }

    for (const auto& arg : arguments) {
        arg->validate(env);
    }

    return true;
}

ExprPtr FuncCallNode::fold(Environment& env) const {
    bool has_unknown = false;
    std::vector<ExprPtr> folded_args;

    for (const auto& arg : arguments) {
        auto folded_arg = arg->fold(env);
        if (dynamic_cast<NumPtr>(folded_arg) == nullptr) {
            has_unknown = true;
        }
        folded_args.push_back(folded_arg);
    }

    // TODO: 如果所有參數都折疊成數字節點，則嘗試對函數調用進行常量折疊
    if (!has_unknown) {
        Environment func_env(&env);  // 為函數調用創建一個新的環境，鏈接到當前環境
        Function func = env.getFunction(func_name->getName());

        for (size_t i = 0; i < folded_args.size(); i++) {
            Value arg_value = folded_args[i]->evaluate(env);
            func_env.setVariable(func.parameters[i], arg_value);
        }

        // 如果是內建函數，直接調用內建函數進行計算
        if (func.is_builtin()) {
            Value result = func.builtin_func(func_env);  // 傳遞當前環境中的變數值給內建函數
            return new NumberNode(getLineno(), result);
        }

        Value result = func.body->evaluate(func_env);
        return new NumberNode(getLineno(), result);
    }

    return new FuncCallNode(getLineno(), func_name, folded_args);
}

Value FuncCallNode::evaluate(Environment& env) const {
    // FIXME: 確認是否正確
    String name = func_name->getName();
    Function func = env.getFunction(name);

    Environment func_env(&env);  // 為函數調用創建一個新的環境，鏈接到當前環境

    for (size_t i = 0; i < func.parameters.size(); i++) {
        Value arg_value = arguments[i]->evaluate(env);
        func_env.setVariable(func.parameters[i], arg_value);
    }

    // 如果是內建函數，直接調用內建函數進行計算
    if (func.is_builtin()) {
        return func.builtin_func(func_env);  // 傳遞當前環境中的變數值給內建函數
    }

    // 對於用戶定義的函數，評估函數體
    return func.body->evaluate(func_env);
}
