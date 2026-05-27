#include "ast/statement/funcdef.hpp"

FuncDefPtr FuncDefNode::clone() const {
    IdPtr cloned_id = getId()->clone();
    std::vector<IdPtr>* cloned_params = new std::vector<IdPtr>();
    for (const auto& param : parameters) {
        cloned_params->push_back(param->clone());
    }
    ExprPtr cloned_body = body->clone();
    return new FuncDefNode(getLineno(), cloned_id, cloned_params, cloned_body);
}
