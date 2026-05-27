#pragma once

#include "env.hpp"
#include "visitor/visitor.hpp"

class Evaluator : public Visitor {
   private:
    Environment* env;
    Value return_value;  // 用於存儲評估結果

   public:
    Evaluator(Environment* environment) : env(environment), return_value(0.0) {}

    void* visit(const NumberNode* node) override;
    void* visit(const IdentifierNode* node) override;
    void* visit(const UnaryOpNode* node) override;
    void* visit(const BinaryOpNode* node) override;
    void* visit(const FuncCallNode* node) override;
    void* visit(const AssignmentNode* node) override;
    void* visit(const FuncDefNode* node) override;
    void* visit(const PlotNode* node) override;
};
