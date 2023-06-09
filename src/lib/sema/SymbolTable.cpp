#include "sema/SymbolTable.hpp"

void SymbolTable::addSymbol(SymbolEntry entrie) { entries.push_back(entrie); }

bool SymbolTable::existSymbol(std::string name, int _level) {
  for (auto it = entries.begin(); it != entries.end(); it++) {
    if (it->getName().compare(name) == 0) {
      if (it->getLevel().first == _level)
        return true;
      if (it->getKind() == "loop_var") {
        return true;
      }
    }
  }
  return false;
}
bool SymbolTable::existSymbol(std::string name) {
  for (auto it = entries.begin(); it != entries.end(); it++) {
    if (it->getName().compare(name) == 0) {
      return true;
    }
  }
  return false;
}
bool SymbolTable::existSymbolKind(std::string name) {
  for (auto it = entries.begin(); it != entries.end(); it++) {
    if (it->getName().compare(name) == 0) {
      std::string symbolKind = it->getKind();
      if (symbolKind.compare("parameter") == 0 ||
          symbolKind.compare("variable") == 0 ||
          symbolKind.compare("loop_var") == 0 ||
          symbolKind.compare("constant") == 0) {
        return true;
      }
    }
  }
  return false;
}
SymbolEntry SymbolTable::getSymbol(std::string name) {
  for (auto it = entries.begin(); it != entries.end(); it++) {
    if (it->getName().compare(name) == 0) {
      return *it;
    }
  }
  return SymbolEntry();
}
std::vector<SymbolEntry> SymbolTable::getSymbolTableList() { return entries; }
