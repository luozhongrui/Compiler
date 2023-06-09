#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                           const char *const p_name, TypeSet *_typeSet,
                           AstNode *_constValue, AstNode *_nextPtr)
    : AstNode{line, col}, name(p_name), typeSet(*_typeSet),
      constantValue(_constValue), next(_nextPtr) {}

void VariableNode::setNext(AstNode *node) { this->next = node; };
AstNode *VariableNode::getNext() { return next; }
AstNode *VariableNode::getSelf() { return this; }
const char *VariableNode::getNameCString() const { return name.c_str(); }
std::string VariableNode::getTyeCString() {
  std::string typeNameS = typeSet.typeNum;
  if (typeSet.dim.size() > 0) {
    typeNameS += " ";
    for (int i = 0; i < int(typeSet.dim.size()); i++) {
      typeNameS += "[";
      typeNameS += std::to_string(typeSet.dim[i]);
      typeNameS += "]";
    }
  }
  return typeNameS;
}
void VariableNode::accept(AstNodeVisitor &p_vsiitor) { p_vsiitor.visit(*this); }
void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = constantValue;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
