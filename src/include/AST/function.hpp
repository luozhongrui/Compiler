#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class FunctionNode : public AstNode {
public:
  FunctionNode(const uint32_t line, const uint32_t col,
               const char *const p_name,
               const char *const _returnType = nullptr,
               AstNode *parameter = nullptr, AstNode *p_body = nullptr,
               AstNode *nextPtr = nullptr);
  ~FunctionNode() = default;

  // void print() override;
  void setNext(AstNode *node) override;
  AstNode *getNext() override;
  AstNode *getSelf() override;
  const char *getNameCString() const;
  const char *getReturnType();
  std::string getFunctionProToType();
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;

private:
  // TODO: name, declarations, return type, compound statement
  std::string name;
  std::string returnType;
  AstNode *parameters;
  AstNode *body;
  AstNode *next;
};

#endif
