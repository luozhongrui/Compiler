#include "AST/ConstantValue.hpp"
#include <string>
// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     int _value)
    : ExpressionNode{line, col}, intValue(_value),
      conType((char *)"integer", {}) {
  intValuePtr = &intValue;
  floatValuePtr = nullptr;
}
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     float _value)
    : ExpressionNode{line, col}, floatValue(_value),
      conType((char *)"real", {}) {
  floatValuePtr = &floatValue;
  intValuePtr = nullptr;
}
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     const char *const _value)
    : ExpressionNode{line, col}, stringValue(_value),
      conType((char *)"string", {}) {
  intValuePtr = nullptr;
  floatValuePtr = nullptr;
  if (stringValue == "true" || stringValue == "false") {
    conType.typeNum = (char *)"boolean";
    booleanValue = stringValue;
  }
}

void ConstantValueNode::setNext(AstNode *node) { this->next = node; }
AstNode *ConstantValueNode::getNext() { return this->next; }
AstNode *ConstantValueNode::getSelf() { return this; }
void ConstantValueNode::accept(AstNodeVisitor &p_visitor) {
  p_visitor.visit(*this);
}

const char *ConstantValueNode::getValue() {
  if (intValuePtr)
    return std::to_string(intValue).c_str();
  if (floatValuePtr)
    return std::to_string(floatValue).c_str();
  if (!booleanValue.empty())
    return booleanValue.c_str();
  return stringValue.c_str();
}
TypeSet ConstantValueNode::getType() { return conType; }
// TODO: You may use code snippets in AstDumper.cpp
// void ConstantValueNode::print() {}
