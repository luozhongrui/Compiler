#ifndef SEMA_SYMBOL_TABLE_H
#define SEMA_SYMBOL_TABLE_H

#include "sema/SymbolEntry.hpp"
#include <string>
#include <vector>
class SymbolTable {
private:
  std::vector<SymbolEntry> entries;
  std::string context; // program function compound for

public:
  SymbolTable(){};
  SymbolTable(std::string _context) : context(_context){};
  ~SymbolTable(){};
  void addSymbol(SymbolEntry entrie);
  bool existSymbol(std::string name, int _level);
  bool existSymbol(std::string name);
  bool existSymbolKind(std::string name);
  std::vector<SymbolEntry> getSymbolTableList();
  SymbolEntry getSymbol(std::string name);
  std::string getContext() { return context; }
};
#endif