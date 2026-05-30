#pragma once

#include "env.hpp"
#include "visitor/visitor.hpp"

class Folder : public Visitor {
   private:
   public:
    Folder(Environment* environment) {}

    // 外部 Driver 呼叫的入口：傳入舊樹根，回傳優化後的新樹根
    ASTPtr fold(ASTPtr node) {
        if (!node) return nullptr;
        return static_cast<ASTPtr>(node->accept(*this));
    }

    void* visit(const NumberNode* node) override;
    void* visit(const IdentifierNode* node) override;
    void* visit(const UnaryOpNode* node) override;
    void* visit(const BinaryOpNode* node) override;
    void* visit(const FuncCallNode* node) override;
    void* visit(const AssignmentNode* node) override;
    void* visit(const FuncDefNode* node) override;
    void* visit(const PlotNode* node) override;
};
