#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                     const char *const _op,
                                     AstNode *_operand,
                                     AstNode *_next)
    : ExpressionNode{line, col}, op(_op), operand(_operand), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void UnaryOperatorNode::print() {}
void UnaryOperatorNode::setNext(AstNode *node) { this->next = node; }
AstNode *UnaryOperatorNode::getNext() { return this->next; }
AstNode *UnaryOperatorNode::getSelf() { return this; }
const char *UnaryOperatorNode::getOpCString() const { return op.c_str(); }
void UnaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  operand->accept(p_visitor);
}
