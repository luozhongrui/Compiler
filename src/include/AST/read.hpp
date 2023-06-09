#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ReadNode : public AstNode {
public:
  ReadNode(const uint32_t line, const uint32_t col, AstNode *_target = nullptr,
           AstNode *_next = nullptr);
  ~ReadNode() = default;

  // void print() override;
  void setNext(AstNode *node);
  AstNode *getNext() override;
  AstNode *getSelf() override;
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable reference
  AstNode *target;
  AstNode *next;
};

#endif
