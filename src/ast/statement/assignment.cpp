#include "ast/statement/assignment.hpp"

AssignPtr AssignmentNode::clone() const {
    IdPtr cloned_id = getId()->clone();
    ExprPtr cloned_value = value->clone();
    return new AssignmentNode(getLineno(), cloned_id, cloned_value);
}
