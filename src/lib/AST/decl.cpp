#include "AST/decl.hpp"
#include <cstdio>
#include <string>
// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col, AstNode *variable,
                   AstNode *next)
    : AstNode{line, col}, variables(variable), next(next) {}

void DeclNode::setNext(AstNode *node) { this->next = node; }
AstNode *DeclNode::getSelf() { return this; }
AstNode *DeclNode::getNext() { return next; }
void DeclNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

void DeclNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = variables;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
AstNode *DeclNode::getVarNode() {
  return variables;
}
