#pragma once

#include "env.hpp"
#include "visitor/visitor.hpp"

class Validator : public Visitor {
   private:
    Environment* env;
    bool has_unknown_variables;  // 用於追蹤是否遇到過未知變數

   public:
    Validator(Environment* environment) : env(environment), has_unknown_variables(false) {}

    void* visit(const NumberNode* node) override;
    void* visit(const IdentifierNode* node) override;
    void* visit(const UnaryOpNode* node) override;
    void* visit(const BinaryOpNode* node) override;
    void* visit(const FuncCallNode* node) override;
    void* visit(const AssignmentNode* node) override;
    void* visit(const FuncDefNode* node) override;
    void* visit(const PlotNode* node) override;
};
