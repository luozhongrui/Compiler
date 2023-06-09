#include "AST/assignment.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                               AstNode *_leftValue, AstNode *_expression,
                               AstNode *_next)
    : AstNode{line, col}, leftValue(_leftValue), expression(_expression),
      next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void AssignmentNode::print() {}
void AssignmentNode::setNext(AstNode *node) { this->next = node; }
AstNode *AssignmentNode::getNext() { return this->next; }
AstNode *AssignmentNode::getSelf() { return this; }
void AssignmentNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}
void AssignmentNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = leftValue;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  dumpy = expression;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
