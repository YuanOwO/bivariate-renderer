#ifndef AST_EXPRESSION_LEAF_HPP
#define AST_EXPRESSION_LEAF_HPP

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

    const char* getClassName() const override { return "NumberNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    LeafPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    Value getValue() const { return value; }
};

class IdentifierNode : public LeafNode {
   private:
    const String name;

   public:
    IdentifierNode(size_t line, const String& n) : LeafNode(line), name(n) {}
    IdentifierNode(size_t line, const char* n) : LeafNode(line), name(n) {}

    const char* getClassName() const override { return "IdentifierNode"; }
    void serialize(std::ostream& os) const override;
    bool validate(Environment& env) override;
    LeafPtr fold(Environment& env) const override;
    Value evaluate(Environment& env) const override;

    const String& getName() const { return name; }
};

#endif  // AST_EXPRESSION_LEAF_HPP
