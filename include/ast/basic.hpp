#define MAGICALLY_STINKY_NUMBER 114514

#ifndef AST_BASIC_HPP
#  define AST_BASIC_HPP

#  include <ostream>
#  include <string>

class Environment;        // forward declare
class ASTNode;            // forward declare
using ASTPtr = ASTNode*;  // 使用傳統指標，因為 Bison 的 %union 不支援智能指標

using Value = double;
using String = std::string;

// 所有 AST 節點的基底類別
class ASTNode {
   private:
    const size_t lineno;  // 用於錯誤報告的行號

   public:
    ASTNode(size_t line) : lineno(line) {}
    virtual ~ASTNode() = default;
    virtual const char* getClassName() const { return "ASTNode"; }
    virtual bool validate(Environment& env) = 0;         // 驗證語義
    virtual void serialize(std::ostream& os) const = 0;  // 序列化為可讀格式
    virtual ASTPtr fold(Environment& env) const = 0;     // 常量折疊
    virtual Value evaluate(Environment& env) const = 0;  // 評估表達式的值

    size_t getLineno() const { return lineno; }
};

std::ostream& operator<<(std::ostream& os, const ASTPtr& node);

#endif  // AST_BASIC_HPP
