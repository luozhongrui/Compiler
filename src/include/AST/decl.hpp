#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
class DeclNode : public AstNode {
public:
  // variable declaration
  DeclNode(const uint32_t line, const uint32_t col, AstNode *variable = nullptr,
           AstNode *next = nullptr);

  ~DeclNode() = default;

  // void print() override;
  void setNext(AstNode *node);
  AstNode *getSelf();
  AstNode *getNext() override;
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;
  AstNode *getVarNode();

private:
  // TODO: variables
  AstNode *variables;
  AstNode *next;
};

#endif
