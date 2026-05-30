#pragma once

#include "ast/statement.hpp"
#include "enums.hpp"

class PlotNode : public StatementNode {
   private:
    const ExprPtr expression;
    const String filename;

   public:
    PlotNode(size_t line, ExprPtr expr, const String& fname)
        : StatementNode(line), expression(expr), filename(fname) {}
    ~PlotNode() {
        delete expression;  // 釋放表達式的記憶體
    }

    void* accept(Visitor& visitor) const override { return visitor.visit(this); }
    PlotPtr clone() const override;

    const char* getClassName() const override { return "PlotNode"; }

    const ExprPtr getExpression() const { return expression; }
    const String& getFilename() const { return filename; }
};
