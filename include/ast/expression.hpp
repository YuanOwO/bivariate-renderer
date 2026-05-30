#pragma once

#include "ast/ast.hpp"

// 所有表達式節點的基底類別
class ExpressionNode : public ASTNode {
   public:
    ExpressionNode(size_t line) : ASTNode(line) {}

    virtual ExprPtr clone() const override = 0;

    virtual const char* getClassName() const override { return "ExpressionNode"; }
};
