#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <string>
#include <vector>
class ConstantValueNode : public ExpressionNode {
public:
  ConstantValueNode(const uint32_t line, const uint32_t col, int _value);
  ConstantValueNode(const uint32_t line, const uint32_t col, float _value);
  ConstantValueNode(const uint32_t line, const uint32_t col,
                    const char *const _value = nullptr);
  ~ConstantValueNode() = default;

  // void print() override;
  AstNode *getNext() override;
  AstNode *getSelf() override;
  void setNext(AstNode *node);
  void accept(AstNodeVisitor &p_visitor) override;
  const char *getValue();
  TypeSet getType();

private:
  // TODO: constant value
  int *intValuePtr;
  float *floatValuePtr;
  // char *booleanValuePtr;
  int intValue;
  float floatValue;
  std::string booleanValue;
  std::vector<int> array;
  std::string stringValue;
  TypeSet conType;
  AstNode *next;
};

#endif
