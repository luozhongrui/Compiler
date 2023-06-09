#include "AST/return.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col,
                       AstNode *_returnValue, AstNode *_next)
    : AstNode{line, col}, returnValue(_returnValue), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void ReturnNode::print() {}
void ReturnNode::setNext(AstNode *node) { this->next = node; }
AstNode *ReturnNode::getNext() { return this->next; }
AstNode *ReturnNode::getSelf() { return this; }
void ReturnNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void ReturnNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  AstNode *dumpy = returnValue;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
