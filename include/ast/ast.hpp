#pragma once

#include <ostream>
#include <string>
#include <vector>

#include "types.hpp"
#include "visitor/visitor.hpp"

// 所有 AST 節點的基底類別
class ASTNode {
   private:
    const size_t lineno;  // 用於錯誤報告的行號

   public:
    ASTNode(size_t line) : lineno(line) {}
    virtual ~ASTNode() = default;

    virtual void* accept(Visitor& visitor) const = 0;  // 接受訪問者模式
    virtual ASTPtr clone() const = 0;                  // 用於深複製 AST 節點

    virtual const char* getClassName() const { return "ASTNode"; }
    size_t getLineno() const { return lineno; }
};

std::ostream& operator<<(std::ostream& os, const ASTPtr& node);
