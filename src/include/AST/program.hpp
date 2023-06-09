#ifndef AST_PROGRAM_NODE_H
#define AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>

class ProgramNode final : public AstNode {
private:
  std::string name;
  AstNode *declNode;
  AstNode *funcNode;
  AstNode *comStat;
  AstNode *next;
  // TODO: return type, declarations, functions, compound statement

public:
  ~ProgramNode() = default;
  ProgramNode(const uint32_t line, const uint32_t col, const char *const p_name,
              AstNode *declNode = nullptr, AstNode *funcNode = nullptr,
              AstNode *comStat = nullptr, AstNode *next = nullptr);

  // visitor pattern version: const char *getNameCString() const;
  // void print() override;
  void setNext(AstNode *node) override;
  AstNode *getSelf() override;
  AstNode *getNext() override;
  const char *getNameCString() const;
  void accept(AstNodeVisitor &p_visitor) override;
  void visitChildNodes(AstNodeVisitor &p_visitor) override;
};

#endif
