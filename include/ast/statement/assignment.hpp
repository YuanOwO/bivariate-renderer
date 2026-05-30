#pragma once

#include "ast/statement.hpp"

class AssignmentNode : public DefinitionNode {
   private:
    const ExprPtr value;

   public:
    AssignmentNode(size_t line, IdPtr var_id, ExprPtr val) : DefinitionNode(line, var_id), value(val) {}
    ~AssignmentNode() { delete value; }

    void* accept(Visitor& visitor) const override { return visitor.visit(this); }
    AssignPtr clone() const override;

    const char* getClassName() const override { return "AssignmentNode"; }
    const ExprPtr getValue() const { return value; }
};
