#ifndef SEMA_SYMBOL_MANAGER_H
#define SEMA_SYMBOL_MANAGER_H

#include "sema/SymbolTable.hpp"
#include <stack>
extern int scopLevel; // scop level
class SymbolManager {
private:
  /* data */
  std::stack<SymbolTable *> tables;
  std::vector<SymbolTable *> backTable;

public:
  SymbolManager(){};
  ~SymbolManager();
  void pushScope(SymbolTable *newScope);
  void popScope();
  SymbolTable *getSybolTable();
  SymbolTable *getLastSybole() {
    if (backTable.size() >= 2)
      return backTable[backTable.size() - 2];
    else
      return new SymbolTable();
  }
  bool existTableSymbol(std::string name, int _level);
  bool existTableSymbol(std::string name);
  bool existSymbolKind(std::string name);
  SymbolEntry getSymbol(std::string name);
  std::vector<SymbolTable *> getTable() { return backTable; }
};
#endif
