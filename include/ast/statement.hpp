#pragma once

#include "ast/ast.hpp"
#include "ast/expression/leaf.hpp"

class StatementNode;
using StmtPtr = StatementNode*;

// 所有語句節點的基底類別
class StatementNode : public ASTNode {
   public:
    StatementNode(size_t line) : ASTNode(line) {}

    virtual StmtPtr clone() const override = 0;

    virtual const char* getClassName() const override { return "StatementNode"; }
};

// 變數、函數定義的基底類別
class DefinitionNode : public StatementNode {
   private:
    IdPtr id;

   public:
    DefinitionNode(size_t line, IdPtr var_id) : StatementNode(line), id(var_id) {}
    virtual ~DefinitionNode() { delete id; }

    virtual const char* getClassName() const override { return "DefinitionNode"; }
    const IdPtr getId() const { return id; }
};
