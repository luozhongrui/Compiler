#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                       const char *const _op,
                                       AstNode *_left,
                                       AstNode *_right,
                                       AstNode *_next)
    : ExpressionNode{line, col}, op(_op), leftOperand(_left),
      rightOperand(_right), next(_next) {}

// TODO: You may use code snippets in AstDumper.cpp
// void BinaryOperatorNode::print() {}
void BinaryOperatorNode::setNext(AstNode *node) { this->next = node; }
AstNode *BinaryOperatorNode::getNext() { return this->next; }
AstNode *BinaryOperatorNode::getSelf() { return this; }
const char *BinaryOperatorNode::getOpCString() const { return op.c_str(); }
void BinaryOperatorNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  // TODO
  leftOperand->accept(p_visitor);
  rightOperand->accept(p_visitor);
}
