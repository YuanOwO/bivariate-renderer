#include "visitor/serializer.hpp"

#include "ast.hpp"

void* Serializer::visit(const NumberNode* node) {
    os << node->getClassName() << "(" << node->getValue() << ")";

    return nullptr;
}

void* Serializer::visit(const IdentifierNode* node) {
    os << node->getClassName() << "(\"" << node->getName() << "\")";

    return nullptr;
}

void* Serializer::visit(const UnaryOpNode* node) {
    os << node->getClassName() << "(\"" << node->getOperatorSymbol() << "\", ";
    node->getOperand()->accept(*this);
    os << ")";

    return nullptr;
}

void* Serializer::visit(const BinaryOpNode* node) {
    os << node->getClassName() << "(\"" << node->getOperatorSymbol() << "\", ";
    node->getLeftOperand()->accept(*this);
    os << ", ";
    node->getRightOperand()->accept(*this);
    os << ")";

    return nullptr;
}

void* Serializer::visit(const FuncCallNode* node) {
    os << node->getClassName() << "(";
    node->getFuncId()->accept(*this);
    os << ", [";
    const auto& args = node->getArguments();
    for (size_t i = 0; i < args.size(); i++) {
        args[i]->accept(*this);
        if (i < args.size() - 1) {
            os << ", ";
        }
    }
    os << "])";

    return nullptr;
}

void* Serializer::visit(const AssignmentNode* node) {
    os << node->getClassName() << "(";
    node->getId()->accept(*this);
    os << ", ";
    node->getValue()->accept(*this);
    os << ")";

    return nullptr;
}

void* Serializer::visit(const FuncDefNode* node) {
    os << node->getClassName() << "(";
    node->getId()->accept(*this);
    os << ", [";
    const auto& params = node->getParameters();
    for (size_t i = 0; i < params.size(); i++) {
        params[i]->accept(*this);
        if (i < params.size() - 1) {
            os << ", ";
        }
    }
    os << "], ";
    node->getBody()->accept(*this);
    os << ")";

    return nullptr;
}

void* Serializer::visit(const PlotNode* node) {
    os << node->getClassName() << "(\"";
    node->getExpression()->accept(*this);
    os << "\", \"" << node->getFilename() << "\")";

    return nullptr;
}
