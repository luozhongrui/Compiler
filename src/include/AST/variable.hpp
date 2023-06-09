#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class VariableNode : public AstNode {
public:
  VariableNode(const uint32_t line, const uint32_t col,
               const char *const p_name, TypeSet *typeSet,
               AstNode *constValue = nullptr, AstNode *nextPtr = nullptr);
  // VariableNode(const uint32_t line, const uint32_t col,
  //              const char *const p_name, const char *const typeName,
  //              AstNode *constValue = nullptr, AstNode *nextPtr = nullptr);
  ~VariableNode() = default;

  void setNext(AstNode *node);
  AstNode *getSelf();
  AstNode *getNext();
  const char *getNameCString() const;
  std::string getTyeCString();
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable name, type, constant value
  std::string name;
  TypeSet typeSet;
  AstNode *constantValue;
  AstNode *next;
};

#endif
