#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class VariableReferenceNode : public ExpressionNode {
public:
  // normal reference
  VariableReferenceNode(const uint32_t line, const uint32_t col,
                        const char *const _name = nullptr,
                        AstNode *_indices = nullptr,
                        AstNode *_next = nullptr
                        /* TODO: name */);
  // array reference
  // VariableReferenceNode(const uint32_t line, const uint32_t col
  //                       /* TODO: name, expressions */);
  ~VariableReferenceNode() = default;

  // void print() override;
  AstNode *getNext() override;
  AstNode *getSelf() override;
  const char *getNameCString() const;
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: variable name, expressions
  std::string name;
  AstNode *indices;
  AstNode *next;
};

#endif
