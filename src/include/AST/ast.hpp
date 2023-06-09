#ifndef AST_AST_NODE_H
#define AST_AST_NODE_H

#include <cstdint>
#include <map>
#include <vector>
class AstNodeVisitor;

struct Location {
  uint32_t line;
  uint32_t col;

  ~Location() = default;
  Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}
};

struct TypeSet {
  char *typeNum;
  std::vector<int> dim;
  TypeSet(char *typeNum, std::vector<int> dim) : typeNum(typeNum), dim(dim) {}
  TypeSet() {}
};
class AstNode {
protected:
  Location location;

public:
  virtual ~AstNode() = 0;
  AstNode(const uint32_t line, const uint32_t col);

  const Location &getLocation() const;

  // virtual void print() = 0;
  virtual void accept(AstNodeVisitor &p_visitor) = 0;
  virtual void visitChildNodes(AstNodeVisitor &p_visitor){};
  virtual AstNode *getNext() { return 0; };
  virtual AstNode *getSelf() { return 0; };
  virtual void setNext(AstNode *node){};
  virtual TypeSet getType() { return TypeSet(); };
  virtual const char *getValue() { return 0; };
  virtual const char *getNameCString() { return 0; };
  virtual const char *getReturnType() { return 0; };
  virtual std::string getTyeCString() { return ""; };
  virtual AstNode *getVarNode() { return 0; };
  virtual std::string getFunctionProToType() { return ""; };
  virtual const char *getOpCString() { return 0; };
};

#endif
