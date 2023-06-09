#include "AST/print.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, AstNode *_target,
                     AstNode *_next)
    : AstNode{line, col}, target(_target), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void PrintNode::print() {}
void PrintNode::setNext(AstNode *node) { this->next = node; }
AstNode *PrintNode::getNext() { return this->next; }
AstNode *PrintNode::getSelf() { return this; }
void PrintNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void PrintNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = target;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
