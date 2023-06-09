#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"

ExpressionNode::ExpressionNode(const uint32_t line, const uint32_t col)
    : AstNode{line, col} {}
void ExpressionNode::accept(AstNodeVisitor &p_visitor) {}
// AstNode *ExpressionNode::getNext(){};
// AstNode *ExpressionNode::getSelf(){};