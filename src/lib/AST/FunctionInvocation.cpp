#include "AST/FunctionInvocation.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char *const _name,
                                               AstNode *_expression)
    : ExpressionNode{line, col}, name(_name), argument(_expression) {}

// TODO: You may use code snippets in AstDumper.cpp
// void FunctionInvocationNode::print() {}
const char *FunctionInvocationNode::getNameCString() const {
  return name.c_str();
}
void FunctionInvocationNode::setNext(AstNode *node) {
  this->next = node;
}
AstNode *FunctionInvocationNode::getNext() { return this->next; }
AstNode *FunctionInvocationNode::getSelf() { return this; }
void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}
void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  AstNode *dumpy = argument;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
