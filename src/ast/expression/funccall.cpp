#include "ast/expression/funccall.hpp"

FuncCallPtr FuncCallNode::clone() const {
    IdPtr cloned_func_id = getFuncId()->clone();
    Vector<ExprPtr>* cloned_args = new Vector<ExprPtr>();
    for (const auto& arg : arguments) {
        cloned_args->push_back(arg->clone());
    }
    return new FuncCallNode(getLineno(), cloned_func_id, cloned_args);
}
