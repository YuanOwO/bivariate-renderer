#pragma once

#include "ast/expression.hpp"
#include "ast/expression/leaf.hpp"

class FuncCallNode;  // forward declare
using FuncCallPtr = FuncCallNode*;

class FuncCallNode : public ExpressionNode {
   private:
    const IdPtr func_name;
    std::vector<ExprPtr> arguments;

   public:
    FuncCallNode(unsigned int line, IdPtr func_name, const std::vector<ExprPtr>* args)
        : ExpressionNode(line), func_name(func_name) {
        if (args != nullptr) {
            arguments = *args;  // 從指標解引用獲取參數列表
            delete args;        // 釋放參數列表的記憶體
        }
    }
    FuncCallNode(unsigned int line, IdPtr func_name, const std::vector<ExprPtr>& args)
        : ExpressionNode(line), func_name(func_name), arguments(args) {}

    const char* getClassName() const override { return "FuncCallNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    ExprPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    const IdPtr getFuncName() const { return func_name; }
    const std::vector<ExprPtr>& getArguments() const { return arguments; }
};
