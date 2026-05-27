#pragma once

#include "ast/statement.hpp"

class AssignmentNode;
using AssignPtr = AssignmentNode*;

class AssignmentNode : public DefinitionNode {
   private:
    ExprPtr value;

   public:
    AssignmentNode(size_t line, IdPtr var_name, ExprPtr val) : DefinitionNode(line, var_name), value(val) {}

    const char* getClassName() const override { return "AssignmentNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    AssignPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    ExprPtr getValue() const { return value; }
};
