#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class IfNode : public AstNode {
public:
  IfNode(const uint32_t line, const uint32_t col,
         AstNode *_expression = nullptr, AstNode *_body = nullptr,
         AstNode *_bodyOfBody = nullptr, AstNode *_next = nullptr
         /* TODO: expression, compound statement, compound statement */);
  ~IfNode() = default;

  void setNext(AstNode *node);
  AstNode *getNext();
  AstNode *getSelf();
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: expression, compound statement, compound statement
  AstNode *condition;
  AstNode *body;
  AstNode *bodyOfElse;
  AstNode *next;
};

#endif
