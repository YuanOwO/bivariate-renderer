#ifndef ENV_HPP
#define ENV_HPP

#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ast.hpp"

class Environment;
using BuiltinFunc = std::function<Value(const Environment&)>;

struct Function {
    std::vector<String> parameters;
    ExprPtr body = nullptr;

    BuiltinFunc builtin_func = nullptr;  // 如果是內建函數，則這個指標不為空
    bool is_builtin() const { return builtin_func != nullptr; }

    Function() = default;
    Function(const std::vector<String>& params, ExprPtr func_body) : parameters(params), body(func_body) {}
    Function(const std::vector<String>& params, BuiltinFunc func) : parameters(params), builtin_func(func) {}
};

// 環境類別：用來存儲變數和函數定義
class Environment {
   protected:
    // 變數名稱到值的映射
    std::unordered_map<String, Value> values;
    // 函數名稱到參數列表和函數體的映射
    std::unordered_map<String, Function> functions;
    // 用於追蹤未知變數，避免重複報錯
    std::unordered_set<String> unknown_variables;

    // 外部環境指針，用於支持作用域鏈
    Environment* outer;

   public:
    Environment(Environment* outerScope = nullptr) : outer(outerScope) {}
    Environment(std::initializer_list<String> unknowns, Environment* outerScope = nullptr)
        : outer(outerScope), unknown_variables(unknowns) {}
    Environment(const std::vector<String>& unknowns, Environment* outerScope = nullptr)
        : outer(outerScope), unknown_variables(unknowns.begin(), unknowns.end()) {}

    void setVariable(const String name, Value value);
    void setFunction(const String name, const std::vector<String>& params, ExprPtr body);
    void setBuiltinFunction(const String name, const std::vector<String>& params, BuiltinFunc func);

    void unsetVariable(const String name) { values.erase(name); }

    bool hasVariable(const String name) const;
    bool hasFunction(const String name) const;
    bool isUnknownVariable(const String name) const;

    bool isDefined(const String name) const {
        return hasVariable(name) || hasFunction(name) || isUnknownVariable(name);
    }

    Value getVariable(const String name) const;
    Function getFunction(const String name) const;
};

Environment initGlobalEnvironment();

#endif  // ENV_HPP
