#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "string"
#include "visitor/AstNodeVisitor.hpp"
class UnaryOperatorNode : public ExpressionNode {
public:
  UnaryOperatorNode(const uint32_t line, const uint32_t col,
                    const char *const _op = nullptr,
                    AstNode *_operand = nullptr,
                    AstNode *_next = nullptr
                    /* TODO: operator, expression */);
  ~UnaryOperatorNode() = default;

  // void print() override;
  AstNode *getNext() override;
  AstNode *getSelf() override;
  const char *getOpCString() const;
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: operator, expression
  std::string op;
  AstNode *operand, *next;
};

#endif
