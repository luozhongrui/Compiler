#include "sema/SymbolManager.hpp"

SymbolManager::~SymbolManager() {
  while (!tables.empty()) {
    delete (tables.top());
    tables.pop();
  }
}
bool SymbolManager::existTableSymbol(std::string name, int _level) {
  for (auto it = backTable.begin(); it != backTable.end(); it++) {
    if ((*it)->existSymbol(name, _level)) {
      return true;
    }
  }
  return false;
}
bool SymbolManager::existTableSymbol(std::string name) {
  for (auto it = backTable.begin(); it != backTable.end(); it++) {
    if ((*it)->existSymbol(name)) {
      return true;
    }
  }
  return false;
}
bool SymbolManager::existSymbolKind(std::string name) {
  for (auto it = backTable.begin(); it != backTable.end(); it++) {
    if ((*it)->existSymbolKind(name)) {
      return true;
    }
  }
  return false;
}
void SymbolManager::pushScope(SymbolTable *newScope) {
  scopLevel += 1;
  tables.push(newScope);
  backTable.push_back(newScope);
}
void SymbolManager::popScope() {
  scopLevel -= 1;
  delete (tables.top());
  tables.pop();
  backTable.pop_back();
}
SymbolTable *SymbolManager::getSybolTable() { return tables.top(); }
SymbolEntry SymbolManager::getSymbol(std::string name) {
  for (auto it = backTable.rbegin(); it != backTable.rend(); it++) {
    if ((*it)->existSymbol(name)) {
      return (*it)->getSymbol(name);
    }
  }
  return SymbolEntry();
}
