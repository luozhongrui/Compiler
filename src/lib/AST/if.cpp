#include "AST/if.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col, AstNode *_expression,
               AstNode *_body, AstNode *_bodyOfBody, AstNode *_next)
    : AstNode{line, col}, condition(_expression), body(_body),
      bodyOfElse(_bodyOfBody), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void IfNode::print() {}
void IfNode::setNext(AstNode *node) { this->next = node; }
AstNode *IfNode::getNext() { return this->next; }
AstNode *IfNode::getSelf() { return this; }
void IfNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void IfNode::visitChildNodes(AstNodeVisitor &p_visitor) {
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
  dumpy = bodyOfElse;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
