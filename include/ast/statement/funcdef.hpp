#pragma once

#include "ast/statement.hpp"

class FuncDefNode;  // forward declare
using FuncDefPtr = FuncDefNode*;

class FuncDefNode : public DefinitionNode {
   private:
    std::vector<IdPtr> parameters;
    const ExprPtr body;

   public:
    FuncDefNode(size_t line, IdPtr func_name, std::vector<IdPtr>* params, ExprPtr func_body)
        : DefinitionNode(line, func_name), body(func_body) {
        if (params != nullptr) {
            parameters = *params;  // 從指標解引用獲取參數列表
            delete params;         // 釋放參數列表的記憶體
        }
    }
    ~FuncDefNode() {
        for (auto param : parameters) {
            delete param;  // 釋放參數的記憶體
        }
        delete body;  // 釋放函數體的記憶體
    }

    void* accept(Visitor& visitor) const override { return visitor.visit(this); }
    FuncDefPtr clone() const override;

    const char* getClassName() const override { return "FuncDefNode"; }

    const std::vector<IdPtr>& getParameters() const { return parameters; }
    const ExprPtr getBody() const { return body; }
};
