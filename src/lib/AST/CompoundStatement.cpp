#include "AST/CompoundStatement.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line,
                                             const uint32_t col,
                                             AstNode *_localVariable,
                                             AstNode *_statement,
                                             AstNode *nextPtr)
    : AstNode{line, col}, localVariable(_localVariable), statements(_statement),
      next(nextPtr) {}

// TODO: You may use code snippets in AstDumper.cpp
// void CompoundStatementNode::print() {}
void CompoundStatementNode::setNext(AstNode *node) { this->next = node; }
AstNode *CompoundStatementNode::getNext() { return this->next; }
AstNode *CompoundStatementNode::getSelf() { return this; }
void CompoundStatementNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}
void CompoundStatementNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = localVariable;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  AstNode *dumpy1 = statements;
  while (dumpy1) {
    // printf("-conpoStat-statement:---%p----\n", dumpy1);
    dumpy1->accept(p_visitor);
    dumpy1 = dumpy1->getNext();
  }
}
