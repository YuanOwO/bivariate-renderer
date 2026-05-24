#include "ast/statement/assignment.hpp"

#include "env.hpp"
#include "errors.hpp"

void AssignmentNode::serialize(std::ostream& os) const {
    os << getClassName() << "(" << getName() << " = " << value << ")";
}

bool AssignmentNode::validate(Environment& env) {
    String name = getName()->getName();

    if (env.isDefined(name)) {
        throw SemanticError("Variable name '" + name + "' is already defined as a variable or function.");
    }

    value->validate(env);

    if (value->hasUnknownVariable()) {
        throw SemanticError("Assignment to variable '" + name +
                            "' contains unknown variable(s) in the value expression.");
    }

    return true;
}

AssignPtr AssignmentNode::fold(Environment& env) const {
    ExprPtr folded_value = value->fold(env);
    return new AssignmentNode(getLineno(), getName(), folded_value);
}

Value AssignmentNode::evaluate(Environment& env) const {
    std::string name = getName()->getName();
    Value result = value->evaluate(env);
    env.setVariable(name, result);
    return result;
}
