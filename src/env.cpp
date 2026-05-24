#include "env.hpp"

#include <cmath>
#include <stdexcept>

#include "ast.hpp"
#include "errors.hpp"

void Environment::setVariable(const String name, Value value) {
    values[name] = value;
}

void Environment::setFunction(const String name, const std::vector<String>& params, ExprPtr body) {
    functions[name] = Function(params, body);
}

void Environment::setBuiltinFunction(const String name, const std::vector<String>& params, BuiltinFunc func) {
    functions[name] = Function(params, func);
}

bool Environment::hasVariable(const String name) const {
    if (values.find(name) != values.end()) {
        return true;
    }

    if (outer) {
        return outer->hasVariable(name);
    }

    return false;
}

bool Environment::hasFunction(const String name) const {
    if (functions.find(name) != functions.end()) {
        return true;
    }

    if (outer) {
        return outer->hasFunction(name);
    }

    return false;
}

bool Environment::isUnknownVariable(const String name) const {
    if (unknown_variables.find(name) != unknown_variables.end()) {
        return true;
    }

    if (outer) {
        return outer->isUnknownVariable(name);
    }

    return false;
}

Value Environment::getVariable(const String name) const {
    auto it = values.find(name);

    if (it != values.end()) {
        return it->second;
    }

    if (outer) {
        return outer->getVariable(name);
    }

    throw RuntimeError("Undefined variable: " + name);
}

Function Environment::getFunction(const String name) const {
    auto it = functions.find(name);

    if (it != functions.end()) {
        return it->second;
    }

    if (outer) {
        return outer->getFunction(name);
    }

    throw RuntimeError("Undefined function: " + name);
}

//////////////////////////////////////////////////

Environment initGlobalEnvironment() {
    Environment env({"x", "y"});  // 全域環境，預設 x 和 y 為未知變數

    // 這裡可以添加一些內建函數，例如 sin、cos、tan 等
    // 目前暫時不實現這些細節

    double pi = 3.14159265358979323846;
    double e = 2.7182818284590452354;
    double phi = (1 + std::sqrt(5)) / 2;  // 黃金比例

    env.setVariable("pi", pi);
    env.setVariable("e", e);
    env.setVariable("phi", phi);

    // 內建函數，暫時不實現具體功能
    env.setBuiltinFunction("sin", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::sin(x);
    });
    env.setBuiltinFunction("cos", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::cos(x);
    });
    env.setBuiltinFunction("tan", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::tan(x);
    });

    env.setBuiltinFunction("asin", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::asin(x);
    });
    env.setBuiltinFunction("acos", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::acos(x);
    });
    env.setBuiltinFunction("atan", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::atan(x);
    });

    env.setBuiltinFunction("ln", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::log(x);
    });
    env.setBuiltinFunction("log", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::log10(x);
    });
    env.setBuiltinFunction("exp", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::exp(x);
    });

    env.setBuiltinFunction("sqrt", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::sqrt(x);
    });
    env.setBuiltinFunction("cbrt", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::cbrt(x);
    });

    env.setBuiltinFunction("abs", {"x"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        return std::abs(x);
    });
    
    env.setBuiltinFunction("max", {"x", "y"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        Value y = env.getVariable("y");
        return std::max(x, y);
    });
    env.setBuiltinFunction("min", {"x", "y"}, [](const Environment& env) -> Value {
        Value x = env.getVariable("x");
        Value y = env.getVariable("y");
        return std::min(x, y);
    });

    return env;
}
