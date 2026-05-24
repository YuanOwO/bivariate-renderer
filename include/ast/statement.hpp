#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP

#include <memory>
#include <string>
#include <vector>

#include "ast/expression/leaf.hpp"
#include "basic.hpp"

class Environment;
class StatementNode;
using StmtPtr = StatementNode*;

// 所有語句節點的基底類別
class StatementNode : public ASTNode {
   public:
    StatementNode(size_t line) : ASTNode(line) {}

    virtual const char* getClassName() const override { return "StatementNode"; }
    virtual StmtPtr fold(Environment& env) const = 0;  // 常量折疊
};

// 變數、函數定義的基底類別
class DefinitionNode : public StatementNode {
   private:
    IdPtr name;

   public:
    DefinitionNode(size_t line, IdPtr var_name) : StatementNode(line), name(var_name) {}

    virtual const char* getClassName() const override { return "DefinitionNode"; }

    IdPtr getName() const { return name; }
};

#endif  // AST_STATEMENT_HPP
