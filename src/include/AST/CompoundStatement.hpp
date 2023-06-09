#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
class CompoundStatementNode : public AstNode {
public:
  CompoundStatementNode(const uint32_t line, const uint32_t col,
                        AstNode *_localVariable = nullptr,
                        AstNode *_statement = nullptr,
                        AstNode *nextPtr = nullptr
                        /* TODO: declarations, statements */);
  ~CompoundStatementNode() = default;

  // void print() override;
  void setNext(AstNode *node) override;
  AstNode *getSelf() override;
  AstNode *getNext() override;
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: declarations, statements
  AstNode *localVariable;
  AstNode *statements;
  AstNode *next;
};

#endif
