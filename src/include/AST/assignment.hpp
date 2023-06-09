#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
class AssignmentNode : public AstNode {
public:
  AssignmentNode(const uint32_t line, const uint32_t col,
                 AstNode *_leftValue = nullptr, AstNode *_expression = nullptr,
                 AstNode *_next = nullptr
                 /* TODO: variable reference, expression */);
  ~AssignmentNode() = default;

  // void print() override;
  void setNext(AstNode *node) override;
  AstNode *getNext() override;
  AstNode *getSelf() override;
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable reference, expression
  AstNode *leftValue;
  AstNode *expression;
  AstNode *next;
};

#endif
