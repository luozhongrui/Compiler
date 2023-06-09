#ifndef SEMA_SEMANTICERROR_H
#define SEMA_SEMANTICERROR_H

#include "visitor/AstNodeInclude.hpp"
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
extern char *code[210];
extern uint32_t if_error;
// need to class store information for error message
// decl
class SemanticError {
private:
  std::vector<std::string> messages;
  std::string generateNotation(int offset);

public:
  SemanticError(){};
  ~SemanticError(){};
  std::vector<std::string> getErrorMessage();
  void errorSymTable(int line, int col, std::string symbolName);
  // var reference
  void errorVarDecl(VariableNode &p_variable);
  void errorVarSymTable(VariableReferenceNode &p_variable_ref);
  void errorVarParameter(VariableReferenceNode &p_variable_ref);
  void errorVarIndexType(int line, int col);
  void errorVarSubscript(VariableReferenceNode &p_variable_ref);

  // binary unary operator
  void errorBinOp(int line, int col, std::string op, std::string left,
                  std::string right);
  void errorUnOp(int line, int col, std::string op, std::string operant);

  // function invocation
  void errorFuncSymTable(FunctionInvocationNode &p_func_invocation);
  void errorFuncSymNotFunc(FunctionInvocationNode &p_func_invocation);
  void errorFuncParameter(FunctionInvocationNode &p_func_invocation);
  void errorFuncExpr(int line, int col, std::string passing,
                     std::string paramter);

  // Print read
  void errorPrintType(int line, int col);
  void errorReadType(int line, int col);
  void errorReadVar(int line, int col);

  // assignment
  void errorAssignArrayType(int line, int col);
  void errorAssignConstType(int line, int col, std::string var);
  void errorAssignVarRefLoop(int line, int col);
  void errorAssignResult(int line, int col);
  void errorAssignLvalue(int line, int col, std::string var, std::string expr);

  // if and while
  void errorIFWhileResult(int line, int col);

  // for
  void errorForInitial(int line, int col);

  // Return
  void errorReturnVoid(int line, int col);
  void errorReturnType(int line, int col, std::string expr, std::string ret);
};

#endif