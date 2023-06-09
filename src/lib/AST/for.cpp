#include "AST/for.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col, AstNode *_loopVarDecl,
                 AstNode *_initialStmt, AstNode *_condition, AstNode *_body,
                 AstNode *_next)
    : AstNode{line, col}, loopVarDecl(_loopVarDecl), initialStmt(_initialStmt),
      condition(_condition), body(_body), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void ForNode::print() {}
void ForNode::setNext(AstNode *node) { this->next = node; }
AstNode *ForNode::getNext() { return this->next; }
AstNode *ForNode::getSelf() { return this; }
void ForNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ForNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  AstNode *dumpy = loopVarDecl;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  dumpy = initialStmt;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  dumpy = condition;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  dumpy = body;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
