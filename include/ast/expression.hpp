#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP

#include "basic.hpp"

class ExpressionNode;  // forward declare
using ExprPtr = ExpressionNode*;

// 所有表達式節點的基底類別
class ExpressionNode : public ASTNode {
   private:
    bool has_unknown_variable = false;  // 用於標記是否包含未知變數

   protected:
    void markUnknownVariable() { has_unknown_variable = true; }

   public:
    ExpressionNode(size_t line) : ASTNode(line) {}

    virtual const char* getClassName() const override { return "ExpressionNode"; }
    virtual ExprPtr fold(Environment& env) const = 0;    // 常量折疊

    bool hasUnknownVariable() const { return has_unknown_variable; }
};

#endif  // AST_EXPRESSION_HPP
