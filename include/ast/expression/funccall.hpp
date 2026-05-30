#pragma once

#include "ast/expression.hpp"
#include "ast/expression/leaf.hpp"

class FuncCallNode : public ExpressionNode {
   private:
    const IdPtr func_id;
    Vector<ExprPtr> arguments;

   public:
    FuncCallNode(unsigned int line, IdPtr id, const Vector<ExprPtr>* args)
        : ExpressionNode(line), func_id(id) {
        if (args != nullptr) {
            arguments = *args;  // 複製指標 vector
            delete args;        // 安全釋放 Bison 產生的暫時 vector 容器
        }
    }

    ~FuncCallNode() {
        delete func_id;
        for (ExprPtr arg : arguments) {
            delete arg;
        }
    }

    void* accept(Visitor& visitor) const override { return visitor.visit(this); }
    FuncCallPtr clone() const override;

    const char* getClassName() const override { return "FuncCallNode"; }
    const IdPtr getFuncId() const { return func_id; }
    const Vector<ExprPtr>& getArguments() const { return arguments; }
};
