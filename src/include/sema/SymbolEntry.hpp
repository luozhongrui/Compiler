#ifndef SEMA_SYMBOL_ENTRY_H
#define SEMA_SYMBOL_ENTRY_H

#include <map>
#include <string>
#include <utility>
#include <vector>
class SymbolEntry {
private:
  std::map<char, int> typeMap = {{'i', 1}, {'r', 2}, {'b', 3}, {'s', 4}};
  std::string name;
  std::string kind;
  std::string type;
  std::string attribute;
  std::pair<int, std::string> level;
  int typeNum;
  std::vector<uint64_t> dim;
  int ifArray;
  std::vector<std::string> attributeList;

public:
  SymbolEntry(std::string _name, std::string _kind, std::string _type,
              std::string _attribute, std::pair<int, std::string> _level);
  SymbolEntry(std::string _name, std::string _type);
  SymbolEntry(){};
  ~SymbolEntry(){};
  void setAttribute(std::string _attribute);
  void setKind(std::string _kind);
  void setLevel(std::pair<int, std::string> _level);
  std::string getName();
  std::string getKind();
  std::string getAttribute();
  std::pair<int, std::string> getLevel();
  std::string getType();
  std::vector<uint64_t> &getDim();
  int getTypeNum();
  int getIfArray();
  std::vector<std::string> &getAttributeList() { return attributeList; }
};
#endif