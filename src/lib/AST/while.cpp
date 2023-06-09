#include "AST/while.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col, AstNode *_condit,
                     AstNode *_body, AstNode *_next)
    : AstNode{line, col}, condition(_condit), body(_body), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void WhileNode::print() {}
void WhileNode::setNext(AstNode *node) { this->next = node; }
AstNode *WhileNode::getNext() { return this->next; }
AstNode *WhileNode::getSelf() { return this; }
void WhileNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void WhileNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  AstNode *dumpy = condition;
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
