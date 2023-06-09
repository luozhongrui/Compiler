#ifndef SEMA_SEMANTIC_ANALYZER_H
#define SEMA_SEMANTIC_ANALYZER_H

#include "sema/SemanticError.hpp"
#include "sema/SymbolManager.hpp"
#include "visitor/AstNodeVisitor.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#define TABLEDUMP                                                              \
  if (opt_syl) {                                                               \
    dumpSymbol(*symbolManager.getSybolTable());                                \
  }
extern char *code[210];
extern uint32_t opt_syl;
extern uint32_t if_error;

struct operandInfo {
  std::string name;
  int ifArray = 0;
  int typeNum;
  std::string type;
  std::vector<int> dim;

  operandInfo() {}
  operandInfo(std::string _name, int _ifArray, int _typeNum, std::string _type,
              std::vector<int> _dim)
      : name(_name), ifArray(_ifArray), typeNum(_typeNum), type(_type),
        dim(_dim) {}
  operandInfo(int _typeNum, std::string _type, int _ifArry)
      : typeNum(_typeNum), type(_type) {}
};

struct constantInfo {
  int line;
  int col;
  std::string value;
  std::string type;
  constantInfo(int _line, int _col, std::string _value, std::string _type)
      : line(_line), col(_col), value(_value), type(_type) {}
};

class SemanticAnalyzer final : public AstNodeVisitor {
private:
  // TODO: something like symbol manager (manage symbol tables)
  //       context manager, return type manager
  int functionPath = 0, forPath = 0;

public:
  ~SemanticAnalyzer() = default;
  SemanticAnalyzer() = default;

  void visit(ProgramNode &p_program) override;
  void visit(DeclNode &p_decl) override;
  void visit(VariableNode &p_variable) override;
  void visit(ConstantValueNode &p_constant_value) override;
  void visit(FunctionNode &p_function) override;
  void visit(CompoundStatementNode &p_compound_statement) override;
  void visit(PrintNode &p_print) override;
  void visit(BinaryOperatorNode &p_bin_op) override;
  void visit(UnaryOperatorNode &p_un_op) override;
  void visit(FunctionInvocationNode &p_func_invocation) override;
  void visit(VariableReferenceNode &p_variable_ref) override;
  void visit(AssignmentNode &p_assignment) override;
  void visit(ReadNode &p_read) override;
  void visit(IfNode &p_if) override;
  void visit(WhileNode &p_while) override;
  void visit(ForNode &p_for) override;
  void visit(ReturnNode &p_return) override;

  void dumpDemarcation(const char chr);
  void dumpSymbol(SymbolTable &table);
  std::pair<int, std::string> getRealType(operandInfo var);
  bool typeIntRealEquivalent(operandInfo left, operandInfo right);
  bool typeInterEquivalent(operandInfo left, operandInfo right);
  bool typeStringEquivalent(operandInfo left, operandInfo right);
  bool typeBoolEquivalent(operandInfo left, operandInfo right);
  
};

#endif
