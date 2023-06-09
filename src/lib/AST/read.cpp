#include "AST/read.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, AstNode *_target,
                   AstNode *_next)
    : AstNode{line, col}, target(_target), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void ReadNode::print() {}
void ReadNode::setNext(AstNode *node) { this->next = node; }
AstNode *ReadNode::getNext() { return this->next; }
AstNode *ReadNode::getSelf() { return this; }
void ReadNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void ReadNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  AstNode *dumpy = target;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
