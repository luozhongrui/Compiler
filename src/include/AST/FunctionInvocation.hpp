#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class FunctionInvocationNode : public ExpressionNode {
public:
  FunctionInvocationNode(const uint32_t line, const uint32_t col,
                         const char *const _name = nullptr,
                         AstNode *_expression = nullptr);
  ~FunctionInvocationNode() = default;

  const char *getNameCString() const;

  // void print() override;
  AstNode *getNext();
  AstNode *getSelf();
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: function name, expressions
  std::string name;
  AstNode *argument;
  AstNode *next;
};

#endif
