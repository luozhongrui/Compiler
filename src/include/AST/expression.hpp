#ifndef AST_EXPRESSION_NODE_H
#define AST_EXPRESSION_NODE_H

#include "AST/ast.hpp"

class ExpressionNode : public AstNode {
public:
  ~ExpressionNode() = default;
  ExpressionNode(const uint32_t line, const uint32_t col)
      : AstNode{line, col} {}
  virtual const char *getConstantValueCString() const { return (char *)""; }

protected:
  // for carrying type of result of an expression
  // TODO: for next assignment
};

#endif
