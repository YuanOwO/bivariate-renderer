#ifndef AST_STATEMENT_PLOT_HPP
#define AST_STATEMENT_PLOT_HPP

#include "ast/statement.hpp"

class PlotNode;  // forward declare
using PlotPtr = PlotNode*;

class PlotNode : public StatementNode {
   private:
    const ExprPtr expression;

   public:
    PlotNode(size_t line, ExprPtr expr) : StatementNode(line), expression(expr) {}

    const char* getClassName() const override { return "PlotNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    PlotPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    const ExprPtr getExpression() const { return expression; }
};

#endif  // AST_STATEMENT_PLOT_HPP
