#ifndef AST_STATEMENT_FUNCDEF_HPP
#define AST_STATEMENT_FUNCDEF_HPP

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
    FuncDefNode(size_t line, IdPtr func_name, const std::vector<IdPtr>& params, ExprPtr func_body)
        : DefinitionNode(line, func_name), parameters(params), body(func_body) {}

    const char* getClassName() const override { return "FuncDefNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    FuncDefPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    const std::vector<IdPtr>& getParameters() const { return parameters; }
    const ExprPtr getBody() const { return body; }
};

#endif  // AST_STATEMENT_FUNCDEF_HPP
