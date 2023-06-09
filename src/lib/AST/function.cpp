#include "AST/function.hpp"
#include <string.h>
// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                           const char *const p_name,
                           const char *const _returnType, AstNode *_parameter,
                           AstNode *_p_body, AstNode *_nextPtr)
    : AstNode{line, col}, name(p_name), returnType(_returnType),
      parameters(_parameter), body(_p_body), next(_nextPtr) {}

const char *FunctionNode::getNameCString() const { return name.c_str(); }
// TODO: You may use code snippets in AstDumper.cpp

void FunctionNode::setNext(AstNode *node) { this->next = node; }
AstNode *FunctionNode::getNext() { return this->next; }
AstNode *FunctionNode::getSelf() { return this; }
void FunctionNode::accept(AstNodeVisitor &p_visitor) { p_visitor.visit(*this); }

const char *FunctionNode::getReturnType() { return returnType.c_str(); }

std::string FunctionNode::getFunctionProToType() {
  std::string proTyep = "(";
  
  AstNode *decl = parameters;
  while (decl) {
    AstNode *var = decl->getVarNode();
    while(var){
    std::string temp = var->getTyeCString();
    proTyep += strndup(temp.c_str(), strlen(temp.c_str()));
    if (var->getNext()) {
      proTyep += ", ";
    }
    var = var->getNext();
    }
    proTyep += ", ";
    decl = decl->getNext();
  }
  proTyep = proTyep.substr(0, proTyep.size()-2);
  proTyep += ")";
  return proTyep;
}
void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
  AstNode *dumpy = parameters;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
  dumpy = body;
  while (dumpy) {
    dumpy->accept(p_visitor);
    dumpy = dumpy->getNext();
  }
}
