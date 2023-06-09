#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ReturnNode : public AstNode {
public:
  ReturnNode(const uint32_t line, const uint32_t col,
             AstNode *_returnValue = nullptr, AstNode *_next = nullptr

             /* TODO: expression */);
  ~ReturnNode() = default;

  // void print() override;
  AstNode *getNext();
  AstNode *getSelf();
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: expression
  AstNode *returnValue;
  AstNode *next;
};

#endif
