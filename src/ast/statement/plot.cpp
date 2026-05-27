#include "ast/statement/plot.hpp"

PlotPtr PlotNode::clone() const {
    ExprPtr cloned_expr = expression->clone();
    return new PlotNode(getLineno(), cloned_expr, filename);
}
