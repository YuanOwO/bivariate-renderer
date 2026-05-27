#pragma once

#include "ast/expression.hpp"

class LeafNode;
class NumberNode;
class IdentifierNode;
using LeafPtr = LeafNode*;
using NumPtr = NumberNode*;
using IdPtr = IdentifierNode*;

class LeafNode : public ExpressionNode {
   public:
    LeafNode(size_t line) : ExpressionNode(line) {}

    virtual const char* getClassName() const override { return "LeafNode"; }
};

class NumberNode : public LeafNode {
   private:
    const Value value;

   public:
    NumberNode(size_t line, double val) : LeafNode(line), value(val) {}

    void* accept(Visitor& visitor) const override { return visitor.visit(this); }
    NumPtr clone() const override;

    const char* getClassName() const override { return "NumberNode"; }
    Value getValue() const { return value; }
};

class IdentifierNode : public LeafNode {
   private:
    const String name;

   public:
    IdentifierNode(size_t line, const String& n) : LeafNode(line), name(n) {}
    IdentifierNode(size_t line, const char* n) : LeafNode(line), name(n) {}

    void* accept(Visitor& visitor) const override { return visitor.visit(this); }
    IdPtr clone() const override;

    const char* getClassName() const override { return "IdentifierNode"; }
    const String& getName() const { return name; }
};
