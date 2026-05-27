#pragma once

class NumberNode;
class IdentifierNode;
class UnaryOpNode;
class BinaryOpNode;
class FuncCallNode;
class AssignmentNode;
class FuncDefNode;
class PlotNode;

class Visitor {
   public:
    virtual ~Visitor() = default;  // 添加虛擬析構函數以確保正確清理資源

    virtual void* visit(const NumberNode* node) = 0;
    virtual void* visit(const IdentifierNode* node) = 0;
    virtual void* visit(const UnaryOpNode* node) = 0;
    virtual void* visit(const BinaryOpNode* node) = 0;
    virtual void* visit(const FuncCallNode* node) = 0;
    virtual void* visit(const AssignmentNode* node) = 0;
    virtual void* visit(const FuncDefNode* node) = 0;
    virtual void* visit(const PlotNode* node) = 0;
};
