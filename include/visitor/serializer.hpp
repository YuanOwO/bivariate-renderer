#pragma once

#include <ostream>

#include "visitor/visitor.hpp"

class Serializer : public Visitor {
   private:
    std::ostream& os;
    size_t indent_level = 0;

    void indent() const {
        for (size_t i = 0; i < indent_level; i++) {
            os << "  ";
        }
    }

   public:
    Serializer(std::ostream& output_stream) : os(output_stream) {}

    void* visit(const NumberNode* node) override;
    void* visit(const IdentifierNode* node) override;
    void* visit(const UnaryOpNode* node) override;
    void* visit(const BinaryOpNode* node) override;
    void* visit(const FuncCallNode* node) override;
    void* visit(const AssignmentNode* node) override;
    void* visit(const FuncDefNode* node) override;
    void* visit(const PlotNode* node) override;
};
