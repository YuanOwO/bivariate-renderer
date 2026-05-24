#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdexcept>

class SyntaxError : public std::runtime_error {
   public:
    SyntaxError(const std::string& message) : std::runtime_error(message) {}
};

class SemanticError : public std::runtime_error {
   public:
    SemanticError(const std::string& message) : std::runtime_error(message) {}
};

class RuntimeError : public std::runtime_error {
   public:
    RuntimeError(const std::string& message) : std::runtime_error(message) {}
};

#endif  // ERRORS_HPP
