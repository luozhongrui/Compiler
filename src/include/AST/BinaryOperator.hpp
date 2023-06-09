#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class BinaryOperatorNode : public ExpressionNode {
public:
  BinaryOperatorNode(const uint32_t line, const uint32_t col,
                     const char *const _op = nullptr, AstNode *_left = nullptr,
                     AstNode *_right = nullptr, AstNode *_next = nullptr
                     /* TODO: operator, expressions */);
  ~BinaryOperatorNode() = default;

  // void print() override;
  AstNode *getNext() override;
  AstNode *getSelf() override;
  const char *getOpCString() const;
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: operator, expressions
  std::string op;
  AstNode *leftOperand, *rightOperand, *next;
};

#endif
