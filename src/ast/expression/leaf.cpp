#include "ast/expression/leaf.hpp"

#include "env.hpp"
#include "errors.hpp"

//////////////////////////////////////////////////

void NumberNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << value << ")";
}

bool NumberNode::validate(Environment& env) {
    // 數字節點總是有效的
    return true;
}

LeafPtr NumberNode::fold(Environment& env) const {
    // 數字節點無法再折疊了
    return new NumberNode(*this);
}

Value NumberNode::evaluate(Environment& env) const {
    return value;
}

//////////////////////////////////////////////////

void IdentifierNode::serialize(std::ostream& os) const {
    os << getClassName() << "(\"" << getName() << "\")";
}

bool IdentifierNode::validate(Environment& env) {
    bool is_known = env.hasVariable(name);
    bool is_unknown = env.isUnknownVariable(name);

    // 如果這個識別碼既不是已知變數，也不是未知變數，則報錯
    if (!is_known && !is_unknown) {
        throw SemanticError("Undefined variable '" + name + "'.");
    }

    if (is_unknown) {
        markUnknownVariable();  // 標記為包含未知變數
    }

    return true;
}

LeafPtr IdentifierNode::fold(Environment& env) const {
    if (env.hasVariable(name)) {
        Value var_value = env.getVariable(name);
        return new NumberNode(getLineno(), var_value);
    }

    // 否則無法折疊，返回自己
    return new IdentifierNode(*this);
}

Value IdentifierNode::evaluate(Environment& env) const {
    if (!env.hasVariable(name)) {
        throw RuntimeError("Undefined variable '" + name + "' at runtime.");
    }

    return env.getVariable(name);
}
