#include "ast/statement/funcdef.hpp"

#include "env.hpp"
#include "errors.hpp"

void FuncDefNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << getName()->getName() << ", [";
    for (size_t i = 0; i < parameters.size(); i++) {
        os << parameters[i]->getName();
        if (i < parameters.size() - 1) {
            os << ", ";
        }
    }
    os << "], " << body << ")";
}

bool FuncDefNode::validate(Environment& env) {
    std::string name = getName()->getName();

    if (env.isDefined(name)) {
        throw SemanticError("Function name '" + name + "' is already defined as a variable or function.");
    }

    std::vector<String> param_names;
    for (const auto& param : parameters) {
        param_names.push_back(param->getName());
    }

    Environment func_env(param_names, &env);  // 為函數體創建一個新的環境，包含參數作為未知變數

    // 驗證函數體
    body->validate(func_env);

    return true;
}

FuncDefPtr FuncDefNode::fold(Environment& env) const {
    std::vector<IdPtr> folded_params;
    for (const auto& param : parameters) {
        folded_params.push_back(new IdentifierNode(*param));  // 參數名稱不變，直接複製
    }
    ExprPtr folded_body = body->fold(env);

    auto ret = new FuncDefNode(getLineno(), getName(), folded_params, folded_body);

    return ret;
}

Value FuncDefNode::evaluate(Environment& env) const {
    std::string func_name = getName()->getName();
    std::vector<String> param_names;
    for (const auto& param : parameters) {
        param_names.push_back(param->getName());
    }
    env.setFunction(func_name, param_names, body);
    return MAGICALLY_STINKY_NUMBER;  // 用於表示特殊狀態的魔法數字
}
