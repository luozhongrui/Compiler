#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class PrintNode : public AstNode {
public:
  PrintNode(const uint32_t line, const uint32_t col, AstNode *_target = nullptr,
            AstNode *_next = nullptr);
  ~PrintNode() = default;

  // void print() override;
  void setNext(AstNode *node);
  AstNode *getNext() override;
  AstNode *getSelf() override;
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: expression
  AstNode *target;
  AstNode *next;
};

#endif
