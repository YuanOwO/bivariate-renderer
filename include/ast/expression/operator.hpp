#ifndef AST_EXPRESSION_OPERATOR_HPP
#define AST_EXPRESSION_OPERATOR_HPP

#include "ast/expression.hpp"
#include "enums.hpp"

class OperatorNode : public ExpressionNode {
   private:
    const OPERATOR op;  // 運算子符號 ID

   public:
    OperatorNode(size_t line, OPERATOR operator_id) : ExpressionNode(line), op(operator_id) {}

    virtual const char* getClassName() const override { return "OperatorNode"; }

    OPERATOR getOperatorId() const { return op; }
    const char* getOperatorSymbol() const;
};

class UnaryOpNode : public OperatorNode {
   private:
    const ExprPtr operand;

   public:
    UnaryOpNode(size_t line, OPERATOR operator_id, ExprPtr operand_node)
        : OperatorNode(line, operator_id), operand(operand_node) {}

    const char* getClassName() const override { return "UnaryOpNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    ExprPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    const ExprPtr getOperand() const { return operand; }
};

class BinaryOpNode : public OperatorNode {
   private:
    const ExprPtr lhs;
    const ExprPtr rhs;

   public:
    BinaryOpNode(size_t line, OPERATOR operator_id, ExprPtr left_node, ExprPtr right_node)
        : OperatorNode(line, operator_id), lhs(left_node), rhs(right_node) {}

    const char* getClassName() const override { return "BinaryOpNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    ExprPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    const ExprPtr getLeftOperand() const { return lhs; }
    const ExprPtr getRightOperand() const { return rhs; }
};

#endif  // AST_EXPRESSION_OPERATOR_HPP
