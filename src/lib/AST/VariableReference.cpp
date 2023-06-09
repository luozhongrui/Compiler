#include "AST/VariableReference.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line,
                                             const uint32_t col,
                                             const char *const _name,
                                             AstNode *_indices,
                                             AstNode *_next)
    : ExpressionNode{line, col}, name(_name), indices(_indices), next(_next) {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
// void VariableReferenceNode::print() {}
void VariableReferenceNode::setNext(AstNode *node) { this->next = node; }
AstNode *VariableReferenceNode::getNext() { return this->next; }
AstNode *VariableReferenceNode::getSelf() { return this; }
const char *VariableReferenceNode::getNameCString() const {
  return name.c_str();
}
void VariableReferenceNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}

void VariableReferenceNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  AstNode *dumpy = indices;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
