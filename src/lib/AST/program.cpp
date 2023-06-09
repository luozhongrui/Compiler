#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                         const char *const p_name, AstNode *_declNode,
                         AstNode *_funcNode, AstNode *_comStat, AstNode *_next)
    : AstNode{line, col}, name(p_name), declNode(_declNode),
      funcNode(_funcNode), comStat(_comStat), next(_next) {}

const char *ProgramNode::getNameCString() const { return name.c_str(); }
void ProgramNode::setNext(AstNode *node) { this->next = node; }

AstNode *ProgramNode::getSelf() { return this; }
AstNode *ProgramNode::getNext() { return next; }
void ProgramNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }
void ProgramNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = declNode;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  AstNode *dumpy1 = funcNode;
  while (dumpy1) {
    dumpy1->accept(p_visitor);
    dumpy1 = dumpy1->getNext();
  }
  comStat->accept(p_visitor);
}
