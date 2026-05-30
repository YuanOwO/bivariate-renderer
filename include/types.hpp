#pragma once

#include <string>
#include <vector>

using Value = double;
using Uint8 = unsigned char;
using String = std::string;

template <typename T>
using Vector = std::vector<T>;

// forward declare

class ASTNode;
class ExpressionNode;
class StatementNode;

class LeafNode;
class NumberNode;
class IdentifierNode;

class FuncCallNode;

class OperatorNode;
class UnaryOpNode;
class BinaryOpNode;

class AssignmentNode;
class FuncDefNode;
class PlotNode;

// 使用傳統指標，因為 Bison 的 %union 不支援智能指標

using ASTPtr = ASTNode*;

using ExprPtr = ExpressionNode*;
using StmtPtr = StatementNode*;

using LeafPtr = LeafNode*;
using NumPtr = NumberNode*;
using IdPtr = IdentifierNode*;

using FuncCallPtr = FuncCallNode*;

using UnaryOpPtr = UnaryOpNode*;
using BinaryOpPtr = BinaryOpNode*;

using AssignPtr = AssignmentNode*;
using FuncDefPtr = FuncDefNode*;
using PlotPtr = PlotNode*;
