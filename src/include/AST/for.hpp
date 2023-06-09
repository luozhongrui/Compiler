#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"

class ForNode : public AstNode {
public:
  ForNode(const uint32_t line, const uint32_t col, AstNode *_loopVarDecl = nullptr,
  AstNode *_initialStmt= nullptr,
  AstNode *_condition= nullptr,
  AstNode *_body= nullptr,
  AstNode *_next= nullptr
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
  ~ForNode() = default;

  // void print() override;
  AstNode *getNext();
  AstNode *getSelf();
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: declaration, assignment, expression, compound statement
  AstNode *loopVarDecl;
  AstNode *initialStmt;
  AstNode *condition;
  AstNode *body;
  AstNode *next;
};

#endif
