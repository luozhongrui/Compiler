#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class WhileNode : public AstNode {
public:
  WhileNode(const uint32_t line, const uint32_t col, AstNode *_condit = nullptr,
            AstNode *_body = nullptr, AstNode *_next = nullptr
            /* TODO: expression, compound statement */);
  ~WhileNode() = default;

  // void print() override;
  AstNode *getNext();
  AstNode *getSelf();
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: expression, compound statement
  AstNode *condition;
  AstNode *body;
  AstNode *next;
};

#endif
