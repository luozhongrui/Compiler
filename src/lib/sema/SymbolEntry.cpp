#include "sema/SymbolEntry.hpp"
// #include <iostream>
#include <string.h>

void trim(std::string &s) {
  if (!s.empty()) {
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
  }
}
std::vector<std::string> split(const std::string &str,
                               const std::string &delim) {
  std::vector<std::string> res;
  if ("" == str)
    return res;
  char *strs = new char[str.length() + 1];
  strcpy(strs, str.c_str());

  char *d = new char[delim.length() + 1];
  strcpy(d, delim.c_str());

  char *p = strtok(strs, d);
  while (p) {
    std::string s = p;
    trim(s);
    res.push_back(s);
    p = strtok(NULL, d);
  }

  return res;
}

SymbolEntry::SymbolEntry(std::string _name, std::string _kind,
                         std::string _type, std::string _attribute,
                         std::pair<int, std::string> _level)
    : name(_name), kind(_kind), type(_type), attribute(_attribute),
      level(_level) {
  typeNum = typeMap[type[0]];
  for (int i = 0; i < int(type.size()); i++) {
    if (type[i] == '[') {
      std::string value = "";
      int j = i;
      while (type[j] != ']') {
        j++;
        value += type[j];
      }
      value = value.substr(0, value.size() - 1);
      dim.push_back(std::stoi(value));
    }
  }
  if (dim.size() > 0) {
    ifArray = 1;
  } else {
    ifArray = 0;
  }
  if (!attribute.empty()) {
    attributeList = split(attribute, ",");
  }
}
SymbolEntry::SymbolEntry(std::string _name, std::string _type)
    : name(_name), type(_type) {
  typeNum = typeMap[type[0]];
  for (int i = 0; i < int(type.size()); i++) {
    if (type[i] == '[') {
      std::string value = "";
      int j = i;
      while (type[j] != ']') {
        j++;
        value += type[j];
      }
      value = value.substr(0, value.size() - 1);
      // printf("---%s----\n", value.c_str());s
      dim.push_back(std::stoi(value));
    }
  }
  if (dim.size() > 0) {
    ifArray = 1;
  } else {
    ifArray = 0;
  }
  if (!attribute.empty()) {
    attributeList = split(attribute, ",");
  }
}

void SymbolEntry::setAttribute(std::string _attribute) {
  attribute = _attribute;
}
void SymbolEntry::setKind(std::string _kind) { kind = _kind; }
void SymbolEntry::setLevel(std::pair<int, std::string> _level) {
  level = _level;
}
std::string SymbolEntry::getName() { return name; }
std::string SymbolEntry::getKind() { return kind; }
std::string SymbolEntry::getAttribute() { return attribute; }
std::pair<int, std::string> SymbolEntry::getLevel() { return level; }
std::string SymbolEntry::getType() { return type; }
std::vector<uint64_t> &SymbolEntry::getDim() { return dim; }
int SymbolEntry::getTypeNum() { return typeNum; }
int SymbolEntry::getIfArray() { return ifArray; }